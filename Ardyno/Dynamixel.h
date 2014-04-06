
#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include <Arduino.h>
#include "StreamController.h"

typedef uint8_t DynamixelID;
typedef uint8_t DynamixelStatus;
typedef uint8_t DynamixelInstruction;

#define BROADCAST_ID 0xFE

struct DynamixelPacket
{
	DynamixelID mID;
	uint8_t mLenght;
	union{
		DynamixelInstruction mInstruction;
		DynamixelStatus mStatus;
	};
	uint8_t *mData;
	uint8_t mCheckSum;
	
	uint8_t checkSum();
};

class DynamixelInterface
{
	public:
	
	
	DynamixelInterface(Stream &aStream, StreamController &aStreamController);
	
	void sendPacket(const DynamixelPacket &aPacket);
	void receivePacket(DynamixelPacket &aPacket);
	
	private:
	
	Stream &mStream;
	StreamController &mStreamController;
};


enum DynInstruction
{
	DYN_PING		=0x01,
	DYN_READ		=0x02,
	DYN_WRITE		=0x03,
	DYN_REG_WRITE	=0x04,
	DYN_ACTION		=0x05,
	DYN_RESET		=0x06,
	DYN_SYNC_WRITE	=0x07
};

#define DYN_INTERNAL_BUFFER_SIZE 32

class DynamixelDevice
{
	public:
	
	DynamixelDevice(DynamixelInterface &aInterface, DynamixelID aId);

	DynamixelStatus ping();
	// warning : array pointed by aPtr must be at least two bytes long
	DynamixelStatus read(uint8_t aAddress, uint8_t aSize, uint8_t *aPtr);
	// warning : one byte must be allocated before aPtr
	DynamixelStatus write(uint8_t aAddress, uint8_t aSize, uint8_t *aPtr);
	// warning : one byte must be allocated before aPtr
	DynamixelStatus regWrite(uint8_t aAddress, uint8_t aSize, uint8_t *aPtr);
	DynamixelStatus action();
	DynamixelStatus reset();
	
	//sizeof(T) must be lower than DYN_INTERNAL_BUFFER_SIZE, and in any case lower than 256
	template<class T>
	inline DynamixelStatus read(uint8_t aAddress, T& aData);
	template<class T>
	inline DynamixelStatus write(uint8_t aAddress, const T& aData);
	template<class T>
	inline DynamixelStatus regWrite(uint8_t aAddress, const T& aData);

	private:
	
	void transaction(uint8_t aInstruction, uint8_t aLenght, uint8_t *aData);
	
	DynamixelInterface &mInterface;
	DynamixelID mID;
	bool mWaitResponse;
	
	DynamixelPacket mPacket;
	
	static uint8_t sInternalBuffer[DYN_INTERNAL_BUFFER_SIZE];
};


template<class T>
DynamixelStatus DynamixelDevice::read(uint8_t aAddress, T& aData)
{
	read(aAddress, uint8_t(sizeof(T)), sInternalBuffer);
	memcpy(&aData, sInternalBuffer, sizeof(T));
}
template<class T>
DynamixelStatus DynamixelDevice::write(uint8_t aAddress, const T& aData)
{
	memcpy(sInternalBuffer+1, &aData, sizeof(T));
	write(aAddress, uint8_t(sizeof(T)), sInternalBuffer+1);
}
template<class T>
DynamixelStatus DynamixelDevice::regWrite(uint8_t aAddress, const T& aData)
{
	memcpy(sInternalBuffer+1, &aData, sizeof(T));
	regWrite(aAddress, uint8_t(sizeof(T)), sInternalBuffer+1);
}

#endif
