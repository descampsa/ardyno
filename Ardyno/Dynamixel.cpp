
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
	mStream.readBytes(header,2);
	mStream.readBytes(reinterpret_cast<char*>(&aPacket), 3);
	mStream.readBytes(reinterpret_cast<char*>(aPacket.mData), aPacket.mLenght-2);
	mStream.readBytes(reinterpret_cast<char*>(&(aPacket.mCheckSum)),1);
}


DynamixelDevice::DynamixelDevice(DynamixelInterface &aInterface, DynamixelID aID):
	mInterface(aInterface), mID(aID), mWaitResponse(true)
{}

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

void DynamixelDevice::transaction(uint8_t aInstruction, uint8_t aLenght, uint8_t *aData)
{
	mPacket.mID=mID;
	mPacket.mInstruction=aInstruction;
	mPacket.mLenght=aLenght+2;
	mPacket.mData=aData;
	mPacket.mCheckSum=mPacket.checkSum();
	mInterface.sendPacket(mPacket);
	if((mWaitResponse || mPacket.mInstruction==DYN_READ) && mPacket.mID!=BROADCAST_ID)
	{
		mInterface.receivePacket(mPacket);
	}
}
