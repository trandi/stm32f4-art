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


#include "timer.h"
#include "Synth.h"
#include "Stepper.h"
#include "AccelerometreControl.h"


// changing the order of the PIns changes the direction of rotation. But the sequence has to be the same.
Stepper stepperRight(B0, B1, C4, C5);
Stepper stepperMiddle(E10, E9, E8, E7);
Stepper stepperLeft(E14, E13, E12, E11);




void accXChange(int16_t newX) {
	float newPos = newX > 0 ? stepperMiddle.getPos() + 0.1f : stepperMiddle.getPos() - 0.1f;
	if(newPos < 0) newPos = 0;
	else if(newPos > 1) newPos = 1;

	stepperMiddle.setPos(newPos);

	synth.setVolume(newPos * 100);
}
void accYChange(int16_t newY) {
	// noop for now
}
void accZChange(int16_t newZ) {
	// noop for now
}

// the below 1000 sensitivity was found empirically to work well
// but it's dependent of the exact accelerometre and its configuration
AccelerometreControl accControl(accXChange, accYChange, accZChange, 1000);



// TODO why do I need these functions ?
// Can't pass these method calls on objects directly as pointers to functions ?
void twoMilliTickForSteppers() {
	stepperRight.twoMilliTick();
	stepperMiddle.twoMilliTick();
	stepperLeft.twoMilliTick();
}
void synthUpdateNote() {
	SynthRandomVars_TypeDef synthVars = synth.updateNoteEach10Millisecs();

	if(synthVars.freq >= 0) {
		stepperLeft.setPos(synthVars.freq);
	}
	if(synthVars.duration >= 0) {
		stepperRight.setPos(synthVars.duration);
	}
}
void checkAccelerometre() {
	accControl.checkValuesChanged();
}


int main(void) {
	/* Initialise LEDs */
	LEDInit(LED_ORANGE);
	LEDInit(LED_GREEN);
	LEDInit(LED_RED);
	LEDInit(LED_BLUE);

	// Green Led On: start of application
	LEDOn(LED_GREEN);

	systickInit();
	addPeriodicCallback(twoMilliTickForSteppers, 2);
	addPeriodicCallback(synthUpdateNote, 10);
	addPeriodicCallback(checkAccelerometre, 10);



	while (1) {
		// Interrupts handlers are doing their job
		// more specifically SysTick_Handler periodically calls the above callbacks

//		stepperLeft.setPos(0);
//		stepperMiddle.setPos(0);
//		stepperRight.setPos(0);
//
//		delay_ms(3000);
//		stepperLeft.setPos(0.5);
//		stepperMiddle.setPos(0.5);
//		stepperRight.setPos(0.5);
//
//		delay_ms(3000);
//		stepperLeft.setPos(1);
//		stepperMiddle.setPos(1);
//		stepperRight.setPos(1);
//		delay_ms(3000);
	}
}




/************************* Below is Useful in case things go wrong ********************************************/

void myDefaultHandler() {
	LEDOn(LED_RED);  //alert : red LED !
	// enters an infinite loop to preserve the system state for examination by a debugger.
	while (1) {
	}
}

void NMI_Handler(void) {
	myDefaultHandler();
}
void HardFault_Handler(void) {
	myDefaultHandler();
}
void MemManage_Handler(void) {
	myDefaultHandler();
}
void BusFault_Handler(void) {
	myDefaultHandler();
}
void UsageFault_Handler(void) {
	myDefaultHandler();
}
void SVC_Handler(void) {
	myDefaultHandler();
}
void DebugMon_Handler(void) {
	myDefaultHandler();
}
void PendSV_Handler(void) {
	myDefaultHandler();
}
