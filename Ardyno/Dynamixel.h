
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

	private:
	
	void transaction(uint8_t aInstruction, uint8_t aLenght, uint8_t *aData);
	
	DynamixelInterface &mInterface;
	DynamixelID mID;
	bool mWaitResponse;
	
	DynamixelPacket mPacket;
};

#endif
