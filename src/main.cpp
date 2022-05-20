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
	
	DuppaEncoder duppa1;
	DuppaEncoder duppa2;
	DuppaLEDRing led1;
	DuppaLEDRing led2;

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
		//			IPUP_ENABLE= INT pin have the pull-up enabled2.
		//			RMOD_X1= Encoder configured as X1.
		//			RGB_ENCODER= type of encoder is RGB, change to STD_ENCODER in case you are using a normal rotary encoder.
		
		// Open device
		if(!duppa1.begin(0x41, config, errnum))
			throw Exception("failed to setup Duppa1 ", errnum);

		// Open device
		if(!duppa2.begin(0x40, config, errnum))
			throw Exception("failed to setup Duppa ", errnum);
  
		// Open device
		if(!led2.begin(0x61, errnum))
			throw Exception("failed to setup LED   ", errnum);
		
		// Open device
		if(!led1.begin(0x60, errnum))
			throw Exception("failed to setup LED  1  ", errnum);
		
		
		// the LEDS are mechanically reversed from the CW movement of the knobs - so reverse them
		// and offset one of them
		led2.setOffset(2, true);
		led1.setOffset(0, true);
	
//		if(!led2.reset())
//			throw Exception("failed to reset LED ");
//
//		if(!led1.reset())
//			throw Exception("failed to reset LED 1");
//
	 
		
		// run one cycle of LEDS  on and off
		for (int i = 0; i < 24; i++) {
			led1.setBLUE(i, 0xff);
			usleep(20 * 1000);
		}
		
		for (int i = 0; i < 24; i++) {
			led1.setBLUE( i, 0);
			usleep(20 * 1000);
		}

		
		for (int i = 0; i < 24; i++) {
			led2.setGREEN(i, 0xff);
			usleep(20 * 1000);
		}
		
		for (int i = 0; i < 24; i++) {
			led2.setGREEN(i, 0);
			usleep(20 * 1000);
		}
			
			
		printf("reading status\n");
		
		// set the knob colors
		if(!duppa2.setColor(0, 255, 0))
			throw Exception("failed to setColor Duppa");
		
		if(!duppa1.setColor(0, 0, 255))
			throw Exception("failed to setColor Duppa1 ");
	
		// loop and look for changes
		
		while(!quit){
			uint8_t status;
			uint8_t status1;
			
			if(!duppa2.updateStatus(status)
				||! duppa1.updateStatus(status1) ){
				printf("readStatus failed\n");
				quit = true;
				continue;
			}
			
			if(status != 0){
				
				static int cntr = 0;
				
				bool cw = false;
				if(duppa2.wasPressed())
					printf("L Pressed \n");
				
				if(duppa2.wasClicked()){
					quit = true;
				}
				
				if(duppa2.wasMoved(cw)){
					//			led2.setGREEN(23-cntr, 0);
					led2.setGREEN(cntr, cw?128:0);
					
					cntr += cw ?1:-1;
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
					
					led1.setBLUE(  (cntr1 % 23), 0);
					cntr1 += cw ?1:-1;
					led1.setBLUE((cntr1 % 23) , 0x7f);
					printf("R Moved %s %d \n", cw? "CW": "CCW", cntr1);
					
					
				}
			}
			
			usleep(2000);
 		}
 
		duppa2.stop();
		duppa1.stop();
		led2.stop();
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
