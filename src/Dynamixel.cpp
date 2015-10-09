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
	mStatus=DYN_STATUS_OK;
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

void DynamixelDevice::communicationSpeed(uint32_t aSpeed)
{
	uint8_t value=2000000/aSpeed-1;
	if(value!=0) // forbid 2MBd rate, because it is out of spec, and can be difficult to undo
	{
		write(DYN_ADDRESS_BAUDRATE, value);
	}
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


void DynamixelInterface::transaction(uint8_t aID, uint8_t aInstruction, uint8_t aLenght, uint8_t *aData, bool aExpectStatus)
{
	//bool response_expected=(mStatusReturnLevel>1 || (mStatusReturnLevel>0 && mPacket.mInstruction==DYN_READ) || mPacket.mInstruction==DYN_PING);
	
	mPacket.mID=aID;
	mPacket.mInstruction=aInstruction;
	mPacket.mLenght=aLenght+2;
	mPacket.mData=aData;
	mPacket.mCheckSum=mPacket.checkSum();
	sendPacket(mPacket);
	if(aExpectStatus)
	{
		receivePacket(mPacket);
	}
	else
	{
		mPacket.mStatus=DYN_STATUS_OK;
	}
}

DynamixelStatus DynamixelInterface::read(uint8_t aID, uint8_t aAddress, uint8_t aSize, uint8_t *aPtr, uint8_t aStatusReturnLevel)
{
	aPtr[0]=aAddress;
	aPtr[1]=aSize;
	transaction(aID, DYN_READ, 2, aPtr, aStatusReturnLevel>0 && aID!=BROADCAST_ID);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::write(uint8_t aID, uint8_t aAddress, uint8_t aSize, uint8_t *aPtr, uint8_t aStatusReturnLevel)
{
	aPtr[-1]=aAddress;
	transaction(aID, DYN_WRITE, aSize+1, const_cast<uint8_t*>(aPtr-1), aStatusReturnLevel>1 && aID!=BROADCAST_ID);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::regWrite(uint8_t aID, uint8_t aAddress, uint8_t aSize, uint8_t *aPtr, uint8_t aStatusReturnLevel)
{
	aPtr[-1]=aAddress;
	transaction(aID, DYN_REG_WRITE, aSize+1, const_cast<uint8_t*>(aPtr-1), aStatusReturnLevel>1 && aID!=BROADCAST_ID);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::ping(uint8_t aID)
{
	transaction(aID, DYN_PING, 0, NULL, true);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::action(uint8_t aID, uint8_t aStatusReturnLevel)
{
	transaction(aID, DYN_ACTION, 0, NULL, aStatusReturnLevel>1 && aID!=BROADCAST_ID);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::reset(uint8_t aID, uint8_t aStatusReturnLevel)
{
	transaction(aID, DYN_RESET, 0, NULL, aStatusReturnLevel>1 && aID!=BROADCAST_ID);
	return mPacket.mStatus;
}


