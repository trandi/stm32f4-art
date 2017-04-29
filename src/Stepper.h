/**
 * Copyright 2017 Dan Oprescu
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 *     
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include "stm32f4xx.h"
#include <vector>


#define STEPS_COUNT 8
const uint8_t steps[STEPS_COUNT] = {0b0001,0b0011,0b0010,0b0110,0b0100,0b1100,0b1000,0b1001};

#define MIDDLE_POS 1000
#define RANGE_STEPS 280


#define PIN_DEF_COUNT	16
typedef enum {
	B0 = 0, B1 = 1, C4 = 2, C5 = 3,
	E7 = 4, E8 = 5, E9 = 6, E10 = 7,
	E11 = 8, E12 = 9, E13 = 10, E14 = 11,
	// LEDs
	D12 = 12, D13 = 13, D14 = 14, D15 = 15
} Pin_TypeDef;

const uint32_t STEPPER_GPIO_CLK[PIN_DEF_COUNT] = {RCC_AHB1Periph_GPIOB, RCC_AHB1Periph_GPIOB, RCC_AHB1Periph_GPIOC, RCC_AHB1Periph_GPIOC,
										  RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOE,
										  RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOE, RCC_AHB1Periph_GPIOE,
										  RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOD};

const uint16_t STEPPER_GPIO_PIN[PIN_DEF_COUNT] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_4, GPIO_Pin_5,
										  GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10,
										  GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14,
										  GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};

GPIO_TypeDef* STEPPER_GPIO[PIN_DEF_COUNT] = {GPIOB, GPIOB, GPIOC, GPIOC,
									 GPIOE, GPIOE, GPIOE, GPIOE,
									 GPIOE, GPIOE, GPIOE, GPIOE,
									 GPIOD, GPIOD, GPIOD, GPIOD};

class Stepper {
public:
	Stepper(Pin_TypeDef pin1, Pin_TypeDef pin2, Pin_TypeDef pin3, Pin_TypeDef pin4) {
		// storing in reverse order as this will make the matching to the steps patter easier
		pins = {pin4, pin3, pin2, pin1};

		// initialise each pin individually
		for(const Pin_TypeDef& pin : pins) {
			RCC_AHB1PeriphClockCmd(STEPPER_GPIO_CLK[pin], ENABLE);
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = STEPPER_GPIO_PIN[pin];
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(STEPPER_GPIO[pin], &GPIO_InitStructure);
		}

		// we ASSUME the indicator starts in the middle
		targetPos = MIDDLE_POS;
		currPos = MIDDLE_POS;
	}


	/**
	 * @newPos position float between 0..1
	 */
	void setPos(float newPos) {
		if(newPos < 0) newPos = 0.f;
		else if (newPos > 1) newPos = 1.f;
		targetPos = (MIDDLE_POS - RANGE_STEPS / 2) + newPos * RANGE_STEPS;
	}

	/**
	 * @return position float between 0..1
	 */
	float getPos() {
		return ((float)currPos - (MIDDLE_POS - RANGE_STEPS / 2)) / RANGE_STEPS;
	}


	// driving these pins is time sensitive so should be done in the millisecond interrupt
	void twoMilliTick() {
		if(targetPos != currPos) {
			if(tickCount >= speed) {
				tickCount = 0;
				step(targetPos > currPos ? 1 : -1);
			} else {
				tickCount +=2;	// 2 millis each time
			}
		} else {
			// we're in the right position, turn off all pins to avoid energising any coils
			// and wasting energy
			digitalWrite(pins, 0x00);
		}
	}


private:
	std::vector<Pin_TypeDef> pins;
	// do a step every "speed" ms / ticks. The lower this value, the higher the actual speed of the motor !
	uint16_t speed = 6;
	volatile uint16_t tickCount = 0;
	volatile uint16_t targetPos, currPos;

	void digitalWrite(std::vector<Pin_TypeDef> pins, uint8_t bitValues) {
		uint8_t shiftedBitValues = bitValues;
		for(const Pin_TypeDef& pin : pins) {
			if(shiftedBitValues & 0x01) {
				GPIO_SetBits(STEPPER_GPIO[pin], STEPPER_GPIO_PIN[pin]);	// pin HIGH
			} else {
				GPIO_ResetBits(STEPPER_GPIO[pin], STEPPER_GPIO_PIN[pin]); // pin LOW
			}
			shiftedBitValues = shiftedBitValues >> 1;
		}
	}

	void step(int8_t dir) {
		currPos += dir;
		digitalWrite(pins, steps[currPos % STEPS_COUNT]);
	}

};



#endif /* STEPPER_H_ */
