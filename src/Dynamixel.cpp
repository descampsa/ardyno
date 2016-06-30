/**
 * \file Dynamixel.cpp
*/

#include "Dynamixel.h"

uint8_t DynamixelPacket::checkSum()
{
	uint8_t result=mID+mLength+mInstruction;
	int n=0;
	if(mAddress!=255)
	{
		result+=mAddress;
		++n;
	}
	if(mDataLength!=255)
	{
		result+=mDataLength;
		++n;
	}
	for(int i=0; i<(mLength-2-n); ++i)
	{
		result+=mData[i];
	}
	for(int i=0; i<mIDListSize; ++i)
	{
		result+=mIDList[i];
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


void DynamixelInterface::transaction(bool aExpectStatus)
{
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
	mPacket=DynamixelPacket(aID, DYN_READ, 4, aPtr, aAddress, aSize);
	transaction(aStatusReturnLevel>0 && aID!=BROADCAST_ID);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::write(uint8_t aID, uint8_t aAddress, uint8_t aSize, const uint8_t *aPtr, uint8_t aStatusReturnLevel)
{
	mPacket=DynamixelPacket(aID, DYN_WRITE, aSize+3, aPtr, aAddress);
	transaction(aStatusReturnLevel>1 && aID!=BROADCAST_ID);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::regWrite(uint8_t aID, uint8_t aAddress, uint8_t aSize, const uint8_t *aPtr, uint8_t aStatusReturnLevel)
{
	mPacket=DynamixelPacket(aID, DYN_REG_WRITE, aSize+3, aPtr, aAddress);
	transaction(aStatusReturnLevel>1 && aID!=BROADCAST_ID);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::syncWrite(uint8_t nID, const uint8_t *aID, uint8_t aAddress, uint8_t aSize, const uint8_t *aPtr, uint8_t aStatusReturnLevel)
{
	mPacket=DynamixelPacket(BROADCAST_ID, DYN_SYNC_WRITE, (aSize+1)*nID+4, aPtr, aAddress, aSize, nID, aID);
	transaction(false);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::ping(uint8_t aID)
{
	mPacket=DynamixelPacket(aID, DYN_PING, 2, NULL);
	transaction(true);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::action(uint8_t aID, uint8_t aStatusReturnLevel)
{
	mPacket=DynamixelPacket(aID, DYN_ACTION, 2, NULL);
	transaction(aStatusReturnLevel>1 && aID!=BROADCAST_ID);
	return mPacket.mStatus;
}

DynamixelStatus DynamixelInterface::reset(uint8_t aID, uint8_t aStatusReturnLevel)
{
	mPacket=DynamixelPacket(aID, DYN_RESET, 2, NULL);
	transaction(aStatusReturnLevel>1 && aID!=BROADCAST_ID);
	return mPacket.mStatus;
}


