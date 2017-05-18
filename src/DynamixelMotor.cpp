#include "DynamixelMotor.h"

DynamixelDevice::DynamixelDevice(DynamixelInterface &aInterface, DynamixelID aID):
	mInterface(aInterface), mID(aID), mStatusReturnLevel(255)
{
	mStatus=DYN_STATUS_OK;
	if(mID==BROADCAST_ID)
		mStatusReturnLevel=0;
}

DynamixelStatus DynamixelDevice::setId(uint8_t aID)
{
	write(DYN_ADDRESS_ID, aID);
	if(mStatus==DYN_STATUS_OK)
		mID=aID;
	return mStatus;
}

DynamixelStatus DynamixelDevice::getStatusReturnLevel(uint8_t & aSRL)
{
	if(mStatusReturnLevel==255)
	{
		read(DYN_ADDRESS_SRL, mStatusReturnLevel);
	}
	aSRL = mStatusReturnLevel;
	return mStatus;
}

DynamixelStatus DynamixelDevice::setStatusReturnLevel(uint8_t aSRL)
{
	write(DYN_ADDRESS_SRL, aSRL);
	if(mStatus==DYN_STATUS_OK)
	{
		mStatusReturnLevel=aSRL;
	}
	return mStatus;
}

DynamixelStatus DynamixelDevice::getModel(uint16_t &aModel)
{
	read(DYN_ADDRESS_ID, aModel);
	return mStatus;
}

DynamixelStatus DynamixelDevice::getFirmwareVersion(uint8_t &aFirmwareVersion)
{
	read(DYN_ADDRESS_FIRMWARE, aFirmwareVersion);
	return mStatus;
}

DynamixelStatus DynamixelDevice::setBaudRate(int32_t aBaudRate)
{
	uint8_t value=2000000/aBaudRate-1;
	if(value!=0) // forbid 2MBd rate, because it is out of spec, and can be difficult to undo
	{
		write(DYN_ADDRESS_BAUDRATE, value);
		return mStatus;
	}
	return DYN_STATUS_INTERNAL_ERROR;
}

DynamixelStatus DynamixelDevice::init()
{
	mStatusReturnLevel=2;
	ping();
	if(mStatus != DYN_STATUS_OK)
	{
		return mStatus;
	}
	read(DYN_ADDRESS_SRL, mStatusReturnLevel);
	if(mStatus & DYN_STATUS_TIMEOUT)
	{
		mStatusReturnLevel=0;
	}
	return DYN_STATUS_OK;
}



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

void DynamixelMotor::goalPosition(uint16_t aPosition)
{
	write(DYN_ADDRESS_GOAL_POSITION, aPosition);
}

void DynamixelMotor::led(uint8_t aState)
{
	write(DYN_ADDRESS_LED, aState);
}

uint16_t DynamixelMotor::currentPosition()
{
	uint16_t currentPosition;
	read(DYN_ADDRESS_CURRENT_POSITION, currentPosition);
	return currentPosition;
}

