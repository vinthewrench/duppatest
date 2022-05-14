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
		
	//	duppa.reset();
	
	 		uint8_t status;

		
		printf("reading status\n");
		
		while(!quit){
	 
			if(!duppa.readStatus(&status)){
				printf("readStatus failed\n");
				quit = true;
				continue;
			}
		 
			if(status != 0){
				bool cw = false;
				
				printf("Status %02x\n", status);
		 
				if(duppa.wasPressed())
					printf("Pressed\n");
	
				if(duppa.wasClicked())
					printf("Clicked\n");
		
				if(duppa.wasMoved(cw)) {
					printf("Moved %s", cw? "CW": "CCW");
				}
				
	
		 
			}
			
			usleep(2000);
			
		}
		
		
		duppa.stop();
		
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
