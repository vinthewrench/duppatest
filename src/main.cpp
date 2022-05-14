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
		
		// Open device
		if(!duppa.begin(0x40, errnum))
			throw Exception("failed to setup Duppa ", errnum);
		
		duppa.reset();
	
		uint8_t lastStatus = 0;
		uint8_t status;

		while(!quit){
			
			
			if(!duppa.readStatus(status)){
				printf("readStatus failed\n");
				quit = true;
				continue;
			}
		 
			if(status != lastStatus){
				printf("Status %02x\n", status);
				lastStatus = status;
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
