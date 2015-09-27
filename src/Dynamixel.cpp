/**
 * \file Dynamixel.cpp
*/

#include "Dynamixel.h"

uint8_t DynamixelPacket::checkSum()
{
	uint8_t result=mID+mLenght+mInstruction;
	for(int i=0; i<(mLenght-2); ++i)
	{
		result+=mData[i];
	}
	return ~result;
}


DynamixelDevice::DynamixelDevice(DynamixelInterface &aInterface, DynamixelID aID):
	mInterface(aInterface), mID(aID), mStatusReturnLevel(255)
{
	mPacket.mStatus=DYN_STATUS_OK;
	if(mID==BROADCAST_ID)
		mStatusReturnLevel=0;
}

uint8_t DynamixelDevice::statusReturnLevel()
{
	if(mStatusReturnLevel==255)
	{
		init();
	}
	return mStatusReturnLevel;
}

void DynamixelDevice::statusReturnLevel(uint8_t aSRL)
{
	write(DYN_ADDRESS_SRL, aSRL);
	if(status()==DYN_STATUS_OK)
	{
		mStatusReturnLevel=aSRL;
	}
}

uint16_t DynamixelDevice::model()
{
	uint16_t result;
	read(DYN_ADDRESS_ID, result);
	return result;
}

uint8_t DynamixelDevice::firmware()
{
	uint8_t result;
	read(DYN_ADDRESS_FIRMWARE, result);
	return result;
}

void DynamixelDevice::communicationSpeed(uint16_t aSpeed)
{
	uint8_t value=2000000/aSpeed-1;
	write(DYN_ADDRESS_BAUDRATE, value);
}

DynamixelStatus DynamixelDevice::ping()
{
	transaction(DYN_PING, 0, NULL);
	return mPacket.mStatus;
}


DynamixelStatus DynamixelDevice::read(uint8_t aAddress, uint8_t aSize, uint8_t *aPtr)
{
	aPtr[0]=aAddress;
	aPtr[1]=aSize;
	transaction(DYN_READ, 2, aPtr);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelDevice::write(uint8_t aAddress, uint8_t aSize, uint8_t *aPtr)
{
	aPtr[-1]=aAddress;
	transaction(DYN_WRITE, aSize+1, const_cast<uint8_t*>(aPtr-1));
	return mPacket.mStatus;
}

DynamixelStatus DynamixelDevice::regWrite(uint8_t aAddress, uint8_t aSize, uint8_t *aPtr)
{
	aPtr[-1]=aAddress;
	transaction(DYN_REG_WRITE, aSize+1, const_cast<uint8_t*>(aPtr-1));
	return mPacket.mStatus;
}

DynamixelStatus DynamixelDevice::action()
{
	transaction(DYN_ACTION, 0, NULL);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelDevice::reset()
{
	transaction(DYN_RESET, 0, NULL);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelDevice::init()
{
	mStatusReturnLevel=2;
	DynamixelStatus status=ping();
	if(status!=DYN_STATUS_OK)
	{
		return status;
	}
	status=read(DYN_ADDRESS_SRL, mStatusReturnLevel);
	if(status&DYN_STATUS_TIMEOUT)
	{
		mStatusReturnLevel=0;
	}
	return DYN_STATUS_OK;
}

uint8_t DynamixelDevice::sInternalBuffer[]={0};

void DynamixelDevice::transaction(uint8_t aInstruction, uint8_t aLenght, uint8_t *aData)
{
	bool response_expected=(mStatusReturnLevel>1 || (mStatusReturnLevel>0 && mPacket.mInstruction==DYN_READ) || mPacket.mInstruction==DYN_PING);
	
	mPacket.mID=mID;
	mPacket.mInstruction=aInstruction;
	mPacket.mLenght=aLenght+2;
	mPacket.mData=aData;
	mPacket.mCheckSum=mPacket.checkSum();
	mInterface.sendPacket(mPacket);
	if(response_expected)
	{
		mInterface.receivePacket(mPacket);
	}
	else
	{
		mPacket.mStatus=DYN_STATUS_OK;
	}
}



