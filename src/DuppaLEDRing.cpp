//
//  DuppaLEDRing.cpp
//  Duppatest
//
//  Created by Vincent Moscaritolo on 5/14/22.
//

#include "DuppaLEDRing.hpp"


enum I2C_Register {
 	REG_IDCODE = 0xFC,
 } ;

DuppaLEDRing::DuppaLEDRing(){
	_isSetup = false;
}

DuppaLEDRing::~DuppaLEDRing(){
	stop();
}

bool DuppaLEDRing::begin(uint8_t deviceAddress){
	int error = 0;
	
	return begin(deviceAddress, error);
}

bool DuppaLEDRing::begin(uint8_t deviceAddress,  int &error){
	
	
	uint8_t idcode = 0;

	if( _i2cPort.begin(deviceAddress, error)
		&& _i2cPort.readByte(REG_IDCODE, idcode)//  && idcode == deviceAddress
 		) {
	 
		
		printf("ID Code %02x = %02x \n",deviceAddress, idcode );
		_isSetup = true;
	}
 
 
	return _isSetup;
}

void DuppaLEDRing::stop(){
	_isSetup = false;
	_i2cPort.stop();
	
	//	LOG_INFO("QwiicTwist(%02x) stop\n",  _i2cPort.getDevAddr());
}

uint8_t	DuppaLEDRing::getDevAddr(){
	return _i2cPort.getDevAddr();
};
