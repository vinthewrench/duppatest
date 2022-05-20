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


#define USE_GPIO_INTERRUPT 1


#if USE_GPIO_INTERRUPT

#if defined(__APPLE__)
// used for cross compile on osx
#include "macos_gpiod.h"
#else
#include <gpiod.h>
#endif

/* this test uses GPIO interrupts
 
 you will need to install the gpiod and the dev libraries befor building
 
 sudo apt-get install gpiod libgpiod-dev
 
 */

#endif


int main(int argc, const char * argv[]) {
	
	DuppaEncoder knob1;
	DuppaEncoder knob2;
	DuppaLEDRing led1;
	DuppaLEDRing led2;
	
	int err = 0;

#if USE_GPIO_INTERRUPT
	
#define GPIOD_TEST_CONSUMER "gpiod-test"

	struct gpiod_chip* 		_chip = NULL;
	struct gpiod_line*  		_line = NULL;

 // setup GPIO27 as connected to the duppa INT line
	
	const char* gpioPath = "/dev/gpiochip0";
	constexpr uint gpioLine	= 27;
	
	printf("setup GPIO lines\n");
	_chip = gpiod_chip_open(gpioPath);
	if(!_chip) {
		printf("Error open GPIO chip(\"%s\") : %s \n",gpioPath,strerror(errno));
	 	goto cleanup;
	}
 
	// get refs to the lines
	_line =  gpiod_chip_get_line(_chip, gpioLine);
	if(!_line){
		printf("Error gpiod_chip_get_line %d: %s \n",  gpioLine, strerror(errno));
		goto cleanup;
	}
	
	struct gpiod_line_request_config gpio_config;
	memset(&gpio_config, 0, sizeof(gpio_config));
	gpio_config.consumer = GPIOD_TEST_CONSUMER;
	gpio_config.request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT;
	gpio_config.flags = GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP;

 	err = gpiod_line_request(_line, &gpio_config, 0);
	if ( err ){
		printf("Error gpiod_line_request %d: %s \n",  gpioLine, strerror(errno));
		goto cleanup;
	}
 
	err =  gpiod_line_request_falling_edge_events(_line, GPIOD_TEST_CONSUMER);
	if ( err ){
		printf("Error gpiod_line_request_falling_edge_events %d: %s \n",  gpioLine, strerror(errno));
		goto cleanup;
	}
 

#endif

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
		
		
		uint8_t interrupt_config =
			DuppaEncoder::PUSHR
		| 	DuppaEncoder::PUSHP
		| 	DuppaEncoder::RINC
		| 	DuppaEncoder::RDEC ;
		
		// Open device
		if(!knob1.begin(0x41, config,interrupt_config, errnum))
			throw Exception("failed to setup knob1 ", errnum);
		
		// Open device
		if(!knob2.begin(0x40, config, interrupt_config, errnum))
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
		if(!knob2.setColor(0, 255, 0))
			throw Exception("failed to setColor Duppa");
		
		if(!knob1.setColor(0, 0, 255))
			throw Exception("failed to setColor knob1 ");
		
		// loop and look for changes
		
		uint8_t status2 = 0;
		uint8_t status1 = 0;

		while(!quit){
	 
	 		// loop until status changes
			for(;;) {
				
				// get status from knobs
				if(! (knob2.updateStatus(status2)
						&& knob1.updateStatus(status1)))
					throw Exception("failed to setColor knob1 ");
	 
				// if any status bit are set process them
				if( (status1 | status2) != 0) break;
	
				// or take a nap

#if USE_GPIO_INTERRUPT
				struct timespec timeout;
				// Timeout of 60 seconds, pass in NULL to wait forever
				timeout.tv_sec = 60;
				timeout.tv_nsec = 0;
				
				// return 0 if wait timed out, -1 if an error occurred, 1 if an event occurred.
				printf("wait for event:  ");
				err = gpiod_line_event_wait(_line, &timeout);
				if(err == -1){
					printf("Error gpiod_line_event_wait \n");
					goto cleanup;
 				}
	 
				printf("%d \n", err);
	
#else
				usleep(2000);
#endif
 			}
	 
	
			// KNOB 1 is blue.
			// we will follow the turn with one blue LED as it moves.
			// think of it as a radio tuning control knob.
			
			if(status1 != 0){
				bool cw = false;
				static uint8_t cntr1 = 0;
				
				if(knob1.wasPressed())
					printf("Knob 1 Pressed \n");
				
				if(knob1.wasClicked())
					printf("Knob1 Clicked \n");
				
				if(knob1.wasMoved(cw)){
					//				printf("R Moved %s ", cw? "CW": "CCW");
					
					led1.setBLUE(  (cntr1 % 23), 0);
					cntr1 += cw ?1:-1;
					led1.setBLUE((cntr1 % 23) , 0x7f);
					printf("Knob1 moved %s %d \n", cw? "CW": "CCW", cntr1);
				}
			}
			
			
			// KNOB 2 is green.
			// we will track the LED with a trail as it increases..
			// think of it as a volume control knob.
			
			if(status2 != 0){
				
				static int cntr = 0;
				
				bool cw = false;
				if(knob2.wasPressed())
					printf("Knob2 Pressed \n");
				
				if(knob2.wasClicked()){
					printf("Knob2 Clicked - Quit test \n");
					quit = true;
				}
				
				if(knob2.wasMoved(cw)){
					led2.setGREEN(cntr, cw?128:0);
					
					cntr += cw ?1:-1;
					if (cntr> 23) cntr = 23;
					else if (cntr < 0) cntr = 0;
					printf("Knob2 moved %s %d \n", cw? "CW": "CCW", cntr);
				}
				//			printf("\n");
			}
			
		}
		
		knob2.stop();
		knob1.stop();
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
	
#if USE_GPIO_INTERRUPT
cleanup:
	if(_line)
		gpiod_line_release (_line);
	
	if(_chip)
		gpiod_chip_close(_chip);
 #endif

	
	return 0;
}
