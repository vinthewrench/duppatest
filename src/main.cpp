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

int main(int argc, const char * argv[]) {

	DuppaEncoder duppa;
	
	try {

		int  errnum = 0;
		
	// setup audio out
	if(!duppa.begin(0x40, errnum))
		throw Exception("failed to setup Duppa ", errnum);

		
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
