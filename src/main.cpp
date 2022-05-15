//
//  main.cpp
//  Duppa
//
//  Created by Vincent Moscaritolo on 5/14/22.
//

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "CommonDefs.hpp"
#include "DuppaEncoder.hpp"
#include "DuppaLEDRing.hpp"

#include <unistd.h>

int main(int argc, const char * argv[]) {
	
	DuppaEncoder duppa;
	DuppaEncoder duppa1;
	DuppaLEDRing led;
	DuppaLEDRing led1;
	
	try {
		
		bool quit = false;
		int  errnum = 0;
		
		uint8_t config = DuppaEncoder::INT_DATA
		| DuppaEncoder::WRAP_DISABLE
		| DuppaEncoder::DIRE_LEFT
		| DuppaEncoder::IPUP_ENABLE
		| DuppaEncoder::RMOD_X1
		| DuppaEncoder::RGB_ENCODER;
		
		
		//		INT_DATA= The register are considered integer.
		//			WRAP_DISABLE= The WRAP option is disabled
		//			DIRE_LEFT= Encoder left direction increase the value
		//			IPUP_ENABLE= INT pin have the pull-up enabled.
		//			RMOD_X1= Encoder configured as X1.
		//			RGB_ENCODER= type of encoder is RGB, change to STD_ENCODER in case you are using a normal rotary encoder.
		
		// Open device
		if(!duppa.begin(0x40, config, errnum))
			throw Exception("failed to setup Duppa ", errnum);
		
		// Open device
		if(!duppa1.begin(0x41, config, errnum))
			throw Exception("failed to setup Duppa1 ", errnum);
		
		// Open device
		if(!led.begin(0x61, errnum))
			throw Exception("failed to setup LED   ", errnum);
		
		// Open device
		if(!led1.begin(0x60, errnum))
			throw Exception("failed to setup LED  1  ", errnum);
		
		if(!led.reset())
			throw Exception("failed to reset LED ");
		
		if(!led1.reset())
			throw Exception("failed to reset LED 1");
		
		if(!led.clearAll())
			throw Exception("failed to clearAll LED ");
		
		if(!led1.clearAll())
			throw Exception("failed to clearAll LED 1");
		
		
#if 1
		led.setConfig(0x01);
		led.SetScaling(0xFF);
		led.GlobalCurrent(010);
		led.PWM_MODE();
		
		led1.setConfig(0x01);
		led1.SetScaling(0xFF);
		led1.GlobalCurrent(010);
		led1.PWM_MODE();
		
#endif
		led.setConfig(0x01);
		
		if(!led.PWMFrequencyEnable(1))
			throw Exception("failed to PWMFrequencyEnable LED");
		
		if(!led.SpreadSpectrum(0b0010110))
			throw Exception("failed to SpreadSpectrum LED");
		//
		if(!led.GlobalCurrent(0xFF))
			throw Exception("failed to GlobalCurrent LED");
		
		if(!led.SetScaling(0xFF))
			throw Exception("failed to SetScaling LED");
		
		//		if(!led.PWM_MODE())
		//			throw Exception("failed to PWM_MODE LED");
		//
		
		led.GlobalCurrent(010);
		led.PWM_MODE();
		//
		//		for (int i = 0; i < 24; i++) {
		//			 led.setGREEN(i, 0xff);
		//			 usleep(10 * 1000);
		//		  }
		//
		//			for (int i = 0; i < 24; i++) {
		//			 led.setGREEN(i, 0);
		//			 usleep(10 * 1000);
		//		  }
		//
		//		for (int i = 0; i < 24; i++) {
		//			 led1.setGREEN(i, 0xff);
		//			 usleep(30 * 1000);
		//		  }
		//
		//		for (int i = 0; i < 24; i++) {
		//			 led1.setGREEN(i, 0);
		//			 usleep(30 * 1000);
		//		  }
		
		
		//		if(!led.clearAll())
		//			throw Exception("failed to clearAll LED ");
		//
		//		if(!led1.clearAll())
		//			throw Exception("failed to clearAll LED 1");
		//
		//		if(!led.GlobalCurrent(0xFF))
		//			throw Exception("failed to GlobalCurrent LED");
		////duppa.reset();
		
		
		printf("reading status\n");
		
		if(!duppa.setColor(0, 255, 0))
			throw Exception("failed to setColor Duppa");
		
		if(!duppa1.setColor(0, 0, 255))
			throw Exception("failed to setColor Duppa1 ");
		
		while(!quit){
			uint8_t status;
			uint8_t status1;
			
			if(!duppa.updateStatus(status)
				||! duppa1.updateStatus(status1) ){
				printf("readStatus failed\n");
				quit = true;
				continue;
			}
			
			if(status != 0){
				
				static int cntr = 0;
				
				bool cw = false;
				if(duppa.wasPressed())
					printf("L Pressed \n");
				
				if(duppa.wasClicked()){
					quit = true;
				}
				
				if(duppa.wasMoved(cw)){
		//			led.setGREEN(23-cntr, 0);
					led.setGREEN(23-cntr, cw?0:128);
	
					cntr += cw ?-1:1;
					if (cntr> 23) cntr = 23;
					else if (cntr < 0) cntr = 0;
				 		printf("L Moved %s %d \n", cw? "CW": "CCW", cntr);
				}
				//			printf("\n");
			}
			
			if(status1 != 0){
				bool cw = false;
				static uint8_t cntr1 = 0;
				
				if(duppa1.wasPressed())
					printf("R Pressed \n");
				
				if(duppa1.wasClicked())
					printf("R Clicked \n");
				
				if(duppa1.wasMoved(cw)){
					//				printf("R Moved %s ", cw? "CW": "CCW");
					
					led1.setBLUE( cntr1 % 23, 0);
					cntr1 += cw ?-1:1;
					led1.setBLUE(cntr1 % 23 , 0x7f);
					
				}
				//			printf("\n");
			}
			
			usleep(2000);
			
		}
		
		
		duppa.stop();
		duppa1.stop();
		led.stop();
		led1.stop();
		
	}
	
	catch ( const Exception& e)  {
		
		// display error on fail..
		
		printf("\tError %d %s\n\n", e.getErrorNumber(), e.what());
	}
	catch (std::invalid_argument& e)
	{
		// display error on fail..
		
		printf("EXCEPTION: %s ",e.what() );
	}
	
	
	
	return 0;
}
