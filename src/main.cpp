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

constexpr uint8_t antiBounceDefault = 1;
 
constexpr uint8_t doubleClickTime = 50;   // 50 * 10 ms




static uint8_t calculateRingCurrent(uint8_t level) {
	uint8_t current = DuppaLEDRing::maxGlobalCurrent();
	
	level = min(static_cast<int>(level), 7);
	
	uint8_t table[] = {1, 1, 2, 4, 5,  6, 8, DuppaLEDRing::maxGlobalCurrent()};
	
	current = table[level];
	
	return current;
	
}

int main(int argc, const char * argv[]) {
	
	DuppaEncoder _rightKnob;
	DuppaEncoder _leftKnob;
	DuppaLEDRing _rightRing;
	DuppaLEDRing _leftRing;
	
	int err = 0;

	const uint32_t kelly_colors[] = {
		0x7F180D, // Strong Reddish Brown
		0x593315, // Deep Yellowish Brown
		0xC10020, // Vivid Red
		0xF13A13, // Vivid Reddish Orange
		0xFF7A5C, // Strong Yellowish Pink
		0xFF6800, // Vivid Orange
		0xFF8E00, // Vivid Orange Yellow
		0xFFB300, // Vivid Yellow
		0xF4C800, // Vivid Greenish Yellow
		0x93AA00, // Vivid Yellowish Green
		0x007D34, // Vivid Green
		0x00FF00, //  Green
		0x232C16, // Dark Olive Green
		0xA6BDD7, // Very Light Blue
		0x00538A, // Strong Blue
		0x803E75, // Strong Purple
		0xF6768E, // Strong Purplish Pink
		0x53377A, // Strong Violet
		0xB32851, // Strong Purplish Red
		0xCEA262, // Grayish Yellow
		0x817066, // Medium Gray
		0xFFFFFF, // White
 	};

#define kelly_colors_count (sizeof(kelly_colors) / sizeof(uint32_t)  )
	
	
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
	
	// setup the line for input and select pull up resistor
	err =  gpiod_line_request_falling_edge_events_flags(_line,
							GPIOD_TEST_CONSUMER,
							GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP);
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
		//			IPUP_ENABLE= INT pin have the pull-up enab_leftRing.
		//			RMOD_X1= Encoder configured as X1.
		//			RGB_ENCODER= type of encoder is RGB, change to STD_ENCODER in case you are using a normal rotary encoder.
		
		
		uint8_t interrupt_config =
			DuppaEncoder::PUSHR
		| 	DuppaEncoder::PUSHP
		| 	DuppaEncoder::PUSHD
		| 	DuppaEncoder::RINC
		| 	DuppaEncoder::RDEC ;
		
		// Open device
		if(!_rightKnob.begin(0x41, config,interrupt_config, errnum))
			throw Exception("failed to setup _rightKnob ", errnum);
		
		// Open device
		if(!_leftKnob.begin(0x40, config, interrupt_config, errnum))
			throw Exception("failed to setup Duppa ", errnum);
		
		
		_rightKnob.setAntiBounce(antiBounceDefault);
		_leftKnob.setAntiBounce(antiBounceDefault);
		
		_rightKnob.setDoubleClickTime(doubleClickTime);
		_leftKnob.setDoubleClickTime(doubleClickTime);

		// Open device
		if(!_leftRing.begin(0x61, errnum))
			throw Exception("failed to setup LED   ", errnum);
		
		// Open device
		if(!_rightRing.begin(0x60, errnum))
			throw Exception("failed to setup LED  1  ", errnum);
		
		_leftRing.reset();
		_rightRing.reset();
	 
		// Set for normal operation
		_rightRing.setConfig(0x01);
		_leftRing.setConfig(0x01);
		
		// full scaling -- control current with global curent
		_rightRing.SetScaling(0xFF);
		_leftRing.SetScaling(0xFF);
		
		// set full bright
		_rightRing.SetGlobalCurrent(DuppaLEDRing::maxGlobalCurrent());
		_leftRing.SetGlobalCurrent(DuppaLEDRing::maxGlobalCurrent());
 
		_rightRing.clearAll();
		_leftRing.clearAll();
		
	
		// the LEDS are mechanically reversed from the CW movement of the knobs - so reverse them
		// and offset one of them
		_leftRing.setOffset(14, true);
		_rightRing.setOffset(0, true);
			
//
//		// dim the knob colors
//		for(int j = 0 ; j < kelly_colors_count ; j ++){
//
//			RGB color = RGB(kelly_colors[j]);
//			printf("%06x \n", kelly_colors[j] );
//			_leftKnob.setColor(color );
//			usleep(40 * 10000);
//		}
//
//
//
//		// do eight cycles dimming..
//		for(int level = 0; level < 8; level ++) {
//
//			uint8_t ledCurrent = calculateRingCurrent(level);
//			_rightRing.SetGlobalCurrent(ledCurrent);
//
//			// run one cycle of LEDS  on and off
//			for (int i = 0; i < 24; i++) {
//				_rightRing.setBLUE(i, 0xff);
//				usleep(20 * 1000);
//			}
//
//			for (int i = 0; i < 24; i++) {
//				_rightRing.setBLUE( i, 0);
//				usleep(20 * 1000);
//			}
//
//		}
//
//		// do eight cycles dimming..
//		for(int level = 0; level < 8; level ++) {
//
//			uint8_t ledCurrent = calculateRingCurrent(level);
//			_leftRing.SetGlobalCurrent(ledCurrent);
//
//			for (int i = 0; i < 24; i++) {
//				_leftRing.setGREEN(i, 0xff);
//				usleep(20 * 1000);
//			}
//
//			for (int i = 0; i < 24; i++) {
//				_leftRing.setGREEN(i, 0);
//				usleep(20 * 1000);
//			}
//		}
//
		
	 		// set the knob colors
		_rightKnob.setColor( RGB::Blue);
		_leftKnob.setColor( RGB::Lime);
	
		// loop and look for changes
		
		uint8_t status2 = 0;
		uint8_t status1 = 0;

		while(!quit){
	 
	 		// loop until status changes
			for(;;) {
				
				// get status from knobs
				if(! (_leftKnob.updateStatus(status2)
						&& _rightKnob.updateStatus(status1)))
					throw Exception("failed to setColor _rightKnob ");
	 
				// if any status bit are set process them
				if( (status1 | status2) != 0) break;
	
				// or take a nap

#if USE_GPIO_INTERRUPT
				struct timespec timeout;
				// Timeout of 60 seconds, pass in NULL to wait forever
				timeout.tv_sec = 10;
				timeout.tv_nsec = 0;
				gpiod_line_event evt;
				
				// gpiod_line_event_wait return 0 if wait timed out,
				// -1 if an error occurred,
				//  1 if an event occurred.
				err = gpiod_line_event_wait(_line, &timeout);
				if(err == -1){
					printf("Error gpiod_line_event_wait \n");
					goto cleanup;
				}
				// gpiod_line_event_wait only blocks until there's an event or a timeout
				// occurs, it does not read the event.
				// call gpiod_line_event_read  to consume the event.
				else if (err == 1) {
					gpiod_line_event_read(_line, &evt);
				}
				else if (err == 0){
//					printf("timeout\n");
				}
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
				
				if(_rightKnob.wasDoubleClicked())
					printf("Knob 1 Double Clicked \n");
		 
				if(_rightKnob.wasPressed())
					printf("Knob 1 Pressed \n");
				
				if(_rightKnob.wasClicked())
					printf("_rightKnob Clicked \n");
				
				if(_rightKnob.wasMoved(cw)){
					//				printf("R Moved %s ", cw? "CW": "CCW");
					
					_rightRing.setBLUE(  (cntr1 % 23), 0);
					cntr1 += cw ?1:-1;
					_rightRing.setBLUE((cntr1 % 23) , 0x7f);
					printf("_rightKnob moved %s %d \n", cw? "CW": "CCW", cntr1);
					
					int offset = cntr1 %  kelly_colors_count;
					RGB color = RGB(kelly_colors[offset]);
			 		_rightKnob.setColor(color );
		 
				}
			}
			
			
			// KNOB 2 is green.
			// we will track the LED with a trail as it increases..
			// think of it as a volume control knob.
			
			if(status2 != 0){
				
				static int cntr = 0;
				
				bool cw = false;
				
				if(_leftKnob.wasDoubleClicked())
					printf("Knob 2 Double Clicked \n");
		
				if(_leftKnob.wasPressed())
					printf("_leftKnob Pressed \n");
				
				if(_leftKnob.wasClicked()){
					printf("_leftKnob Clicked - Quit test \n");
					quit = true;
				}
				
				if(_leftKnob.wasMoved(cw)){
					_leftRing.setGREEN(cntr, cw?128:0);
					
					cntr += cw ?1:-1;
					if (cntr> 23) cntr = 23;
					else if (cntr < 0) cntr = 0;
					printf("_leftKnob moved %s %d \n", cw? "CW": "CCW", cntr);
					
					
				}
				//			printf("\n");
			}
			
		}
		
		_leftKnob.stop();
		_rightKnob.stop();
		_leftRing.stop();
		_rightRing.stop();
		

		
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
