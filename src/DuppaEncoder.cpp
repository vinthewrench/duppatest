//
//  DuppaEncoder.cpp
//  Duppatest
//
//  Created by Vincent Moscaritolo on 5/14/22.
//

#include "DuppaEncoder.hpp"


DuppaEncoder::DuppaEncoder(){
	_isSetup = false;
}

DuppaEncoder::~DuppaEncoder(){
	stop();
}

bool DuppaEncoder::begin(uint8_t deviceAddress){
	int error = 0;
	
	return begin(deviceAddress, error);
}

bool DuppaEncoder::begin(uint8_t deviceAddress,   int &error){
	
	if( _i2cPort.begin(deviceAddress, error)
//		&& _i2cPort.writeWord(TWIST_COUNT, 0)
//		&& _i2cPort.writeByte(TWIST_STATUS,  0)
		)
	{
		_isSetup = true;
	}
	
 
	return _isSetup;
}

void DuppaEncoder::stop(){
	_isSetup = false;
	_i2cPort.stop();
	
	//	LOG_INFO("QwiicTwist(%02x) stop\n",  _i2cPort.getDevAddr());
}
