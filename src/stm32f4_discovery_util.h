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
  

#ifndef __STM32F4_DISCOVERY_UTIL_H
#define __STM32F4_DISCOVERY_UTIL_H

#ifdef __cplusplus
 extern "C" {
#endif
                                              
#include "stm32f4xx.h"
   

typedef enum 
{
  LED_GREEN = 0,
  LED_ORANGE = 1,
  LED_RED = 2,
  LED_BLUE = 3
} Led_TypeDef;

typedef enum 
{  
  BUTTON_USER = 0,
} Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;     


#define LEDn                             4

#define LED_GREEN_PIN                         GPIO_Pin_12
#define LED_GREEN_GPIO_PORT                   GPIOD
#define LED_GREEN_GPIO_CLK                    RCC_AHB1Periph_GPIOD
  
#define LED_ORANGE_PIN                        GPIO_Pin_13
#define LED_ORANGE_GPIO_PORT                  GPIOD
#define LED_ORANGE_GPIO_CLK                   RCC_AHB1Periph_GPIOD
  
#define LED_RED_PIN                  	      GPIO_Pin_14
#define LED_RED_GPIO_PORT                     GPIOD
#define LED_RED_GPIO_CLK                      RCC_AHB1Periph_GPIOD
  
#define LED_BLUE_PIN                          GPIO_Pin_15
#define LED_BLUE_GPIO_PORT                    GPIOD
#define LED_BLUE_GPIO_CLK                     RCC_AHB1Periph_GPIOD


#define BUTTONn                          1  

/**
 * @brief Wakeup push-button
 */
#define USER_BUTTON_PIN                GPIO_Pin_0
#define USER_BUTTON_GPIO_PORT          GPIOA
#define USER_BUTTON_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define USER_BUTTON_EXTI_LINE          EXTI_Line0
#define USER_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define USER_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource0
#define USER_BUTTON_EXTI_IRQn          EXTI0_IRQn 


GPIO_TypeDef* GPIO_PORT[LEDn] = {LED_GREEN_GPIO_PORT, LED_ORANGE_GPIO_PORT, LED_RED_GPIO_PORT, LED_BLUE_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED_GREEN_PIN, LED_ORANGE_PIN, LED_RED_PIN, LED_BLUE_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED_GREEN_GPIO_CLK, LED_ORANGE_GPIO_CLK, LED_RED_GPIO_CLK, LED_BLUE_GPIO_CLK};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT };

const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN };

const uint32_t BUTTON_CLK[BUTTONn] = {USER_BUTTON_GPIO_CLK };

const uint16_t BUTTON_EXTI_LINE[BUTTONn] = {USER_BUTTON_EXTI_LINE };

const uint8_t BUTTON_PORT_SOURCE[BUTTONn] = {USER_BUTTON_EXTI_PORT_SOURCE};

const uint8_t BUTTON_PIN_SOURCE[BUTTONn] = {USER_BUTTON_EXTI_PIN_SOURCE };
const uint8_t BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn };


void LEDInit(Led_TypeDef Led) {
	  GPIO_InitTypeDef  GPIO_InitStructure;

	  /* Enable the GPIO_LED Clock */
	  RCC_AHB1PeriphClockCmd(GPIO_CLK[Led], ENABLE);

	  /* Configure the GPIO_LED pin */
	  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
}

void LEDOn(Led_TypeDef Led) {
	GPIO_PORT[Led]->BSRRL = GPIO_PIN[Led];
}

void LEDOff(Led_TypeDef Led) {
	GPIO_PORT[Led]->BSRRH = GPIO_PIN[Led];
}

void LEDToggle(Led_TypeDef Led) {
	GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}

void PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode) {
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the BUTTON Clock */
	RCC_AHB1PeriphClockCmd(BUTTON_CLK[Button], ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure Button pin as input */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = BUTTON_PIN[Button];
	GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStructure);

	if (Button_Mode == BUTTON_MODE_EXTI) {
		/* Connect Button EXTI Line to Button GPIO Pin */
		SYSCFG_EXTILineConfig(BUTTON_PORT_SOURCE[Button],
				BUTTON_PIN_SOURCE[Button]);

		/* Configure Button EXTI line */
		EXTI_InitStructure.EXTI_Line = BUTTON_EXTI_LINE[Button];
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* Enable and set Button EXTI Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = BUTTON_IRQn[Button];
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

		NVIC_Init(&NVIC_InitStructure);
	}
}

uint32_t PBGetState(Button_TypeDef Button) {
	return GPIO_ReadInputDataBit(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4_DISCOVERY_UTIL_H */
