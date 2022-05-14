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

	void reset();
 
	 
	uint8_t	getDevAddr();
 
	
private:
 
 
	
	I2C 		_i2cPort;
	bool		_isSetup;

 
};
 
