//
//  DuppaLEDRing.hpp
//  Duppatest
//
//  Created by Vincent Moscaritolo on 5/14/22.
//
 
#pragma once

#include "I2C.hpp"

using namespace std;

class DuppaLEDRing
{
 
public:
 
	DuppaLEDRing();
	~DuppaLEDRing();
 
 
	bool begin(uint8_t deviceAddress);
	bool begin(uint8_t deviceAddress, int &error);
	void stop();

	bool reset();
 
	bool clearAll();

	uint8_t	getDevAddr();
 
	
private:
 
	bool  selectBank(uint8_t b);
	bool	PWM_MODE(void) ;
	

	
	I2C 		_i2cPort;
	bool		_isSetup;

 
};
 
