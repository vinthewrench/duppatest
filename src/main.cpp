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
#include <unistd.h>

int main(int argc, const char * argv[]) {
	
	DuppaEncoder duppa;
	DuppaEncoder duppa1;

	try {
		
		bool quit = false;
		int  errnum = 0;
		
		uint8_t config = DuppaEncoder::INT_DATA
		| DuppaEncoder::WRAP_DISABLE
		| DuppaEncoder::DIRE_RIGHT
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
	
	//duppa.reset();
	 
		
		printf("reading status\n");
		
		duppa.setColor(0, 255, 0);
	
		duppa.setColor(0, 0, 255);
	
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
				bool cw = false;
				if(duppa.wasPressed())
					printf("L Pressed ");
				
				if(duppa.wasClicked())
					printf("L Clicked ");
				
				if(duppa.wasMoved(cw))
					printf("L Moved %s ", cw? "CW": "CCW");
				
				printf("\n");
			}
	
			if(status1 != 0){
				bool cw = false;
				if(duppa1.wasPressed())
					printf("R Pressed ");
				
				if(duppa1.wasClicked())
					printf("R Clicked ");
				
				if(duppa1.wasMoved(cw))
					printf("R Moved %s ", cw? "CW": "CCW");
				printf("\n");
			}

			usleep(2000);
			
		}
		
		
		duppa.stop();
		duppa1.stop();
	
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
