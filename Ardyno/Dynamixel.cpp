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


DynamixelInterface::DynamixelInterface(Stream &aStream, StreamController &aStreamController):
	mStream(aStream), mStreamController(aStreamController)
{
	mStreamController.readMode();
	mStream.setTimeout(1); // return delay time is never more than 1 millisecond
}

void DynamixelInterface::sendPacket(const DynamixelPacket &aPacket)
{
	mStreamController.writeMode();
	
	mStream.write(0xFF);
	mStream.write(0xFF);
	mStream.write(aPacket.mID);
	mStream.write(aPacket.mLenght);
	mStream.write(aPacket.mInstruction);
	if(aPacket.mLenght>2)
	{
		mStream.write(aPacket.mData, aPacket.mLenght-2);
	}
	mStream.write(aPacket.mCheckSum);
	mStream.flush();
	
	mStreamController.readMode();
}

void DynamixelInterface::receivePacket(DynamixelPacket &aPacket)
{
	static char header[2];
	if(mStream.readBytes(header,2)<2)
	{
		aPacket.mStatus=DYN_STATUS_COM_ERROR | DYN_STATUS_TIMEOUT;
		return;
	}
	if(mStream.readBytes(reinterpret_cast<char*>(&aPacket), 3)<3)
	{
		aPacket.mStatus=DYN_STATUS_COM_ERROR | DYN_STATUS_TIMEOUT;
		return;
	}
	if(mStream.readBytes(reinterpret_cast<char*>(aPacket.mData), aPacket.mLenght-2)<(aPacket.mLenght-2))
	{
		aPacket.mStatus=DYN_STATUS_COM_ERROR | DYN_STATUS_TIMEOUT;
		return;
	}
	if(mStream.readBytes(reinterpret_cast<char*>(&(aPacket.mCheckSum)),1)<1)
	{
		aPacket.mStatus=DYN_STATUS_COM_ERROR | DYN_STATUS_TIMEOUT;
		return;
	}
	if(aPacket.checkSum()!=aPacket.mCheckSum)
	{
		aPacket.mStatus=DYN_STATUS_COM_ERROR | DYN_STATUS_CHECKSUM_ERROR;
	}
}


DynamixelDevice::DynamixelDevice(DynamixelInterface &aInterface, DynamixelID aID):
	mInterface(aInterface), mID(aID), mStatusReturnLevel(2)
{
	mPacket.mStatus=DYN_STATUS_OK;
	if(mID!=BROADCAST_ID)
	{
		init();
	}
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
	bool response_expected=(mStatusReturnLevel>1 || (mStatusReturnLevel>0 && mPacket.mInstruction==DYN_READ) || mPacket.mInstruction==DYN_PING) && 
							mPacket.mID!=BROADCAST_ID;
	if(response_expected || (mPacket.mInstruction=!DYN_PING && mPacket.mInstruction!=DYN_READ) )
	{
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
	else
	{
		mPacket.mStatus=DYN_STATUS_COM_ERROR | DYN_STATUS_INVALID_OPERATION;
	}
}

