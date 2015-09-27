#include "DynamixelMotor.h"


DynamixelMotor::DynamixelMotor(DynamixelInterface &aInterface, DynamixelID aId):
	DynamixelDevice(aInterface, aId)
{}


void DynamixelMotor::wheelMode()
{
	jointMode(0,0);
}

void DynamixelMotor::jointMode(uint16_t aCWLimit, uint16_t aCCWLimit)
{
	uint32_t data= (aCWLimit | (uint32_t(aCCWLimit)<<16));
	write(DYN_ADDRESS_CW_LIMIT, data);
}

void DynamixelMotor::enableTorque(bool aTorque)
{
	write(DYN_ADDRESS_ENABLE_TORQUE, uint8_t(aTorque?1:0));
}

void DynamixelMotor::speed(int16_t aSpeed)
{
	if(aSpeed<0)
		aSpeed=-aSpeed | 1024;
	write(DYN_ADDRESS_GOAL_SPEED, aSpeed);
}

void DynamixelMotor::position(uint16_t aPosition)
{
	write(DYN_ADDRESS_GOAL_POSITION, aPosition);
}
