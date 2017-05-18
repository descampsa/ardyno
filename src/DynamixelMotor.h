/**
 * \file DynamixelMotor.h
 * \brief Define classe for dynamixel motor
*/

#ifndef DYNAMIXEL_MOTOR_H
#define DYNAMIXEL_MOTOR_H

#include "DynamixelInterface.h"

class DynamixelDevice
{
	public:
	
	DynamixelDevice(DynamixelInterface &aInterface, DynamixelID aId);
	
	DynamixelStatus init();
	
	DynamixelStatus status()
	{
		return mStatus;
	}
	
	DynamixelStatus getId(DynamixelID &aID)
	{
		aID = mID;
		return mStatus;
	}
	
	DynamixelStatus setId(uint8_t aID);
	
	DynamixelStatus getStatusReturnLevel(uint8_t & aSRL);
	DynamixelStatus setStatusReturnLevel(uint8_t aSRL);
	
	DynamixelStatus getModel(uint16_t &aModel);
	DynamixelStatus getFirmwareVersion(uint8_t &aFirmwareVersion);
	
	DynamixelStatus setBaudRate(int32_t aBaudRate);
	
	
	template<class T>
	inline DynamixelStatus read(uint8_t aAddress, T& aData)
	{
		return mStatus=mInterface.read<T>(mID, aAddress, aData, mStatusReturnLevel);
	}
	
	template<class T>
	inline DynamixelStatus write(uint8_t aAddress, const T& aData)
	{
		return mStatus=mInterface.write<T>(mID, aAddress, aData, mStatusReturnLevel);
	}
	
	template<class T>
	inline DynamixelStatus regWrite(uint8_t aAddress, const T& aData)
	{
		return mStatus=mInterface.regWrite<T>(mID, aAddress, aData, mStatusReturnLevel);
	}
	
	DynamixelStatus ping()
	{
		return mStatus=mInterface.ping(mID);
	}
	
	DynamixelStatus action()
	{
		return mStatus=mInterface.action(mID, mStatusReturnLevel);
	}
	
	DynamixelStatus reset()
	{
		return mStatus=mInterface.reset(mID, mStatusReturnLevel);
	}
	
	private:
	
	DynamixelInterface &mInterface;
	DynamixelID mID;
	uint8_t mStatusReturnLevel;
	DynamixelStatus mStatus;
};


class DynamixelMotor:public DynamixelDevice
{
	public:
	
	DynamixelMotor(DynamixelInterface &aInterface, DynamixelID aId);
	
	void wheelMode();
	void jointMode(uint16_t aCWLimit=0, uint16_t aCCWLimit=0x3FF);
	
	void enableTorque(bool aTorque=true);
	void speed(int16_t aSpeed);
	void goalPosition(uint16_t aPosition);
	
	void led(uint8_t aState);

	uint16_t currentPosition();
};

#endif
