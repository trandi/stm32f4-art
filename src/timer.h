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

#ifndef TIMER_H_
#define TIMER_H_

#include <stm32f4_discovery_util.h>
#include <vector>


/************** SYS TICKing at 1kHz / every 1ms *****************/
void systickInit() {
	// 1 ms tick. Should get 0 if function succeeded
	if (SysTick_Config(SystemCoreClock / 1000)) {
		// Alert RED LED, and also capture error
		LEDOn(LED_RED);
		while (1) {
		}
	}
}


typedef struct {
	void (*f)(void);
	uint16_t period;
	uint32_t lastRun = 0;
} Callback_TypeDef;

std::vector<Callback_TypeDef> callbacks;
void addPeriodicCallback(void (*f)(void), uint16_t period) {
	Callback_TypeDef callback;
	callback.f = f;
	callback.period = period;
	callbacks.push_back(callback);
}

static volatile uint32_t ticks;
extern "C" void SysTick_Handler(void) {
	ticks++;
	for(Callback_TypeDef& callback : callbacks) {
		if(ticks - callback.lastRun >= callback.period) {
			callback.f();
			callback.lastRun = ticks;
		}
	}
}

// return the system clock as milliseconds
inline uint32_t millis(void) {
	return ticks;
}

void delay_ms(uint32_t t) {
	uint32_t end = millis() + t;
	while (millis() < end) {
		// do nothing
	}
}



#endif /* TIMER_H_ */
