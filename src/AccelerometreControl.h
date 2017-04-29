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
#ifndef ACCELEROMETRECONTROL_H_
#define ACCELEROMETRECONTROL_H_

#include <stdlib.h>
#include "tm_stm32f4_lis302dl_lis3dsh.h"

class AccelerometreControl {
public:
	AccelerometreControl(void (*xChange)(int16_t),
			void (*yChange)(int16_t),
			void (*zChange)(int16_t),
			int16_t sensitivity)
	{
		_xChange = xChange;
		_yChange = yChange;
		_zChange = zChange;
		_sensitivity = sensitivity;

		/* Detect proper device */
		if (TM_LIS302DL_LIS3DSH_Detect() == TM_LIS302DL_LIS3DSH_Device_LIS302DL) {
			// This actually probably will NOT work properly with the rest of the values that expect a 9G sensitivity
			TM_LIS302DL_LIS3DSH_Init(TM_LIS302DL_Sensitivity_2_3G, TM_LIS302DL_Filter_2Hz);
		} else if (TM_LIS302DL_LIS3DSH_Detect() == TM_LIS302DL_LIS3DSH_Device_LIS3DSH) {
			TM_LIS302DL_LIS3DSH_Init(TM_LIS3DSH_Sensitivity_16G,	TM_LIS3DSH_Filter_800Hz);
		}

	}

	// checking the accelerometre is time sensitive so should be done at regular intervals
	void checkValuesChanged() {
		TM_LIS302DL_LIS3DSH_ReadAxes(&accData);

		uint32_t currentTime = millis();

		if(abs(accData.X - prevAccData.X) > _sensitivity) {
			// "debounce" by imposing 1 second between changes
			if(currentTime - lastTicksX > 1000) {
				lastTicksX = currentTime;
				_xChange(accData.X);
			}
		}
		if(abs(accData.Y - prevAccData.Y) > _sensitivity) {
			// "debounce" by imposing 1 second between changes
			if(currentTime - lastTicksY > 1000) {
				lastTicksY = currentTime;
				_yChange(accData.Y);
			}
		}
		if(abs(accData.Z - prevAccData.Z) > _sensitivity) {
			// "debounce" by imposing 0.5 second between changes
			if(currentTime - lastTicksZ > 500) {
				lastTicksZ = currentTime;
				_zChange(accData.Z);
			}
		}

		prevAccData = accData;
	}

private:
	TM_LIS302DL_LIS3DSH_t accData, prevAccData;
	uint32_t lastTicksX = 0, lastTicksY = 0, lastTicksZ = 0;
	int16_t _sensitivity;
	void (*_xChange)(int16_t);
	void (*_yChange)(int16_t);
	void (*_zChange)(int16_t);
};



#endif /* ACCELEROMETRECONTROL_H_ */
