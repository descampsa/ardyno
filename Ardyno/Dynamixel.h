/**
 * \file Dynamixel.h
 * \brief Define classes for dynamixel protocol
*/

#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include <Arduino.h>
#include "StreamController.h"

/** \brief Type of dynamixel device ID */
typedef uint8_t DynamixelID;
/** \brief Type of dynamixel status code */
typedef uint8_t DynamixelStatus;
/** \brief Type of dynamixel instructions */
typedef uint8_t DynamixelInstruction;

/** \brief ID for broadcast */
#define BROADCAST_ID 0xFE

/**
 * \struct DynamixelPacket
 * \brief Struct of a dynamixel packet (instruction or status)
*/
struct DynamixelPacket
{
	/** \brief Packet ID */
	DynamixelID mID;
	/** \brief Packet lenght (2 + parameter size) */
	uint8_t mLenght;
	/** \brief Packet instruction or status */
	union{
		DynamixelInstruction mInstruction;
		DynamixelStatus mStatus;
	};
	/** \brief Pointer to packet parameter (or NULL if no parameter) */
	uint8_t *mData;
	/** \brief Packet checksum */
	uint8_t mCheckSum;
	
	/**
	 * \brief Compute checksum of the packet
	 * \return Checksum value
	*/
	uint8_t checkSum();
};

/**
 * \class  DynamixelInterface
 * \brief Represent a dynamixel bus
*/
class DynamixelInterface
{
	public:
	
	/**
	 * \brief Constructor
	 * \param[in] aStream : underlying stream (e.g. HardwareSerial or SoftSerial)
	 * \param[in] aStreamController : stream controller related to stream
	*/
	DynamixelInterface(Stream &aStream, StreamController &aStreamController);
	
	/**
	 * \brief Send a packet on bus
	 * \param[in] aPacket : Packet to send
	 * The function wait for the packet to be completly sent (using Stream.flush)
	*/
	void sendPacket(const DynamixelPacket &aPacket);
	/**
	 * \brief Receive a packet on bus
	 * \param[out] aPacket : Received packet. mData field must be previously allocated.
	 * The function wait for a new packet on the bus. Timeout depends of timeout of the underlying stream.
	 * \todo Return error code in case of communication error (timeout, checksum error, ...)
	*/
	void receivePacket(DynamixelPacket &aPacket);
	
	private:
	
	Stream &mStream;
	StreamController &mStreamController;
};

/**
	\brief Dynamixel intruction values
*/
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
