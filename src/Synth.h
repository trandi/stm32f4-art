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

#ifndef SYNTH_H_
#define SYNTH_H_

#include "timer.h"

/* For the audio codec to work as we want configure stm32f4_discovery_audio_codec.h to:
 - replace #define AUDIO_MAL_MODE_NORMAL with #define AUDIO_MAL_MODE_CIRCULAR
 - enable DMA HALF transfer complete interrupt (#define AUDIO_MAL_DMA_IT_HT_EN) in addition to the full transfer complete (#define AUDIO_MAL_DMA_IT_TC_EN)
 */
#include "stm32f4_discovery_audio_codec.h"
#include "saw_osc.h"


#define SAMPLERATE              48000
#define BUFF_LEN                384  /* Audio buffer length : count in 16bits half-words */
#define BUFF_LEN_DIV4           96 // 2ms latency at 48kHz
#define BUFF_LEN_DIV2           192
#define FREQ1                   440.0f   // default carrier frequency
#define ON                      1
#define OFF                     0

#define MAX_RND 4294967294.0f	// 2^32-2


typedef struct SynthRandomVars_TypeDef {
	float	freq = -1.f;
	float	duration = -1.f;
} SynthRandomVars_TypeDef;


class Synth {
public:
	Synth() {
		/* Initialize User Button */
		PBInit(BUTTON_USER, BUTTON_MODE_GPIO);

		/* Initialise the onboard random number generator !  Will be used by randomNum() */
		RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
		RNG_Cmd(ENABLE);

		sawtooth_active(SAMPLERATE);

		EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, 60, SAMPLERATE);
		EVAL_AUDIO_Play(audiobuff, BUFF_LEN);
	}


	uint32_t setVolume(uint8_t newVolume) {
		return EVAL_AUDIO_VolumeCtl(newVolume);
	}

	/**
	 * Time sensitive, expects to be called every 10ms exactly.
	 * @duration how long in milliseconds to last
	 * @return the random values between 0..1 used to generate the new note and its duration.
	 * 			Negative is no new value has been generated yet.
	 */
	SynthRandomVars_TypeDef updateNoteEach10Millisecs() {
		SynthRandomVars_TypeDef res;

		if(remainingDuration > 0) {
			remainingDuration -= 10;
		} else {
			// update the duration between each note
			float durationFactor = randomNum();
			remainingDuration = 4000 * durationFactor;
			res.duration = durationFactor;

			// switch between main and decreasing sound
			if(state == ON) {
				state = OFF;
				pass = 0.f;
			} else {
				state = ON;
				pass = 0.5f;

				// choose a random note
				float freqFactor = randomNum();
				f1 = 2000.0f * freqFactor + 50.0f;

				res.freq = freqFactor;
			}
		}

		return res;
	}

	void halfTransfer() {
		sawtooth_runproc(audiobuff, 0, BUFF_LEN_DIV4, pass, f1);
	}

	void transferComplete() {
		sawtooth_runproc(audiobuff, BUFF_LEN_DIV2, BUFF_LEN_DIV4, pass, f1);
	}

private:
	uint16_t audiobuff[BUFF_LEN] = { 0 };  // The circular audio buffer
	volatile float f1 = FREQ1;
	float pass = 1.f;
	uint8_t state = OFF;
	volatile int16_t remainingDuration = 1000;


	// returns a random float between 0 and 1
	float randomNum(void) {
		float random = 1.0f;
		if (RNG_GetFlagStatus(RNG_FLAG_DRDY) == SET) {
			random = (float) (RNG_GetRandomNumber() / MAX_RND);
		}
		return random;
	}

};




Synth synth;



/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 *
 * Manages the DMA Half Transfer complete interrupt.
 * Enabled by defining AUDIO_MAL_DMA_IT_HT_EN
 *
 * This function is called when half of the requested buffer has been transferred
 * This callback is useful in Circular mode only (when AUDIO_MAL_MODE_CIRCULAR
 * define is enabled)
 */
void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size) {
	/* Generally this interrupt routine is used to load the buffer when
	 * a streaming scheme is used: When first Half buffer is already transferred load
	 * the new data to the first half of buffer while DMA is transferring data from
	 * the second half. And when Transfer complete occurs, load the second half of
	 * the buffer while the DMA is transferring from the first half ... */

	synth.halfTransfer();
}

/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 *
 * Manages the DMA Transfer complete interrupt.
 * Enabled by defining AUDIO_MAL_DMA_IT_TC_EN
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size) {
	synth.transferComplete();
}


/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void) {
	/* UNUSED, we're are using DMA transfer and hence the 2 above "HalfTransfer" and "TransferComplete" callbacks */
	return 0;
}

uint32_t Codec_TIMEOUT_UserCallback(void) {
	LEDOn(LED_RED); // alert : red LED !
	return 0;
}



#endif /* SYNTH_H_ */
