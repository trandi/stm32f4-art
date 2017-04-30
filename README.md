# stm32f4-art
Generate random notes with a sawtooth oscillator, display the config on repurposed car dashboard needles and control the whole with knocks (using the accelerometre).

See it in action here:  
https://trandi.wordpress.com/2017/05/01/stm32f4-discovery-art/

Many thanks and credits to :
- Xavier Halgand for the inspiration and code for the sound generation bit : https://github.com/MrBlueXav/horrorophone-eclipse-with-makefile  
- Tilen Majerle for his library the made my life easier when using the onboard accelerometer: http://stm32f4-discovery.com/2015/03/library-53-gpio-for-stm32f4  


Needs "-std=c++11 or -std=gnu++11" for compilation.

Also, in my case I was using Atollic TrueSTUDIO :  
	- doesn't support ST-LINK under Linux -> use J-LINK from SEGGER  
	- STLinkReflash application to put this new firmware  
	- then OpenOCD doesn't support this to flash .elf files  
	- install JLink_Linux_V614c_x86_64.deb  
	- this will provide JLinkExe that can connect  
	- use SWD rather than JTAG to make it work  
	- J-Link>loadfile toto.hex   


stm32f4_discovery_audio_codec.h  
	- replace #define AUDIO_MAL_MODE_NORMAL with #define AUDIO_MAL_MODE_CIRCULAR  
	- enable DMA HALF transfer complete interrupt (#define AUDIO_MAL_DMA_IT_HT_EN) in addition to the full transfer complete (#define AUDIO_MAL_DMA_IT_TC_EN)  
