//
//  DuppaEncoder.hpp
//  Duppatest
//
//  Created by Vincent Moscaritolo on 5/14/22.
//

#pragma once

#include "I2C.hpp"

using namespace std;

class DuppaEncoder
{
 
public:
	DuppaEncoder();
	~DuppaEncoder();
 
 
	bool begin(uint8_t deviceAddress = 0x3F);
	bool begin(uint8_t deviceAddress,  int &error);
	void stop();
 
	uint8_t	getDevAddr();
 
private:
 
 
	
	I2C 		_i2cPort;
	bool		_isSetup;

};
 
