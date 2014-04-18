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
	 *
	 * The function wait for the packet to be completly sent (using Stream.flush)
	*/
	void sendPacket(const DynamixelPacket &aPacket);
	/**
	 * \brief Receive a packet on bus
	 * \param[out] aPacket : Received packet. mData field must be previously allocated
	 *
	 * The function wait for a new packet on the bus. Timeout depends of timeout of the underlying stream.
	 * \todo Return error code in case of communication error (timeout, checksum error, ...)
	*/
	void receivePacket(DynamixelPacket &aPacket);
	
	private:
	
	Stream &mStream;
	StreamController &mStreamController;
};

/**
 * \brief Dynamixel intruction values
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

/**
 * \brief Dynamixel control table addresses (only addresses used by all models)
*/
enum DynCommonAddress
{
	/** \brief Model number, uint16_t , read only */
	DYN_ADDRESS_MODEL		=0x00,
	/** \brief Firmware version, uint8_t, read only */
	DYN_ADDRESS_FIRMWARE	=0x02,
	/** \brief Device ID, uint8_t, writable */
	DYN_ADDRESS_ID			=0x03,
	/** \brief Communication baudrate, uint8_t, writable */
	DYN_ADDRESS_BAUDRATE	=0x04,
	/** \brief Return Delay Time , uint8_t, writable */
	DYN_ADDRESS_RDT			=0x05,
	/** \brief Status Return Level , uint8_t, writable 
	 *
	 * Define when the device will send back a status packet :
	 * 0 : Ping only
	 * 1 : Read and ping
	 * 2 : All instructions
	*/
	DYN_ADDRESS_SRL			=0x10 
};

/**
 * \brief Dynamixel model number values
*/
enum DynModel
{
	DYN_MODEL_AX12A	=0x0C,
	DYN_MODEL_AX12W	=0x2C,
	DYN_MODEL_AX18A	=0x12,
	
	DYN_MODEL_DX113	=0x71,
	DYN_MODEL_DX114	=0x74,
	DYN_MODEL_DX117	=0x75,
	
	DYN_MODEL_RX10	=0x0A,
	DYN_MODEL_RX24F	=0x18,
	DYN_MODEL_RX28	=0x1C,
	DYN_MODEL_RX64	=0x40,
	
	DYN_MODEL_EX106	=0x6B,
	
	DYN_MODEL_MX12W	=0x68,
	DYN_MODEL_MX28T	=0x1D,
	DYN_MODEL_MX28R	=0x1D,
	DYN_MODEL_MX64T	=0x36,
	DYN_MODEL_MX64R	=0x36,
	DYN_MODEL_MX106T=0x40,
	DYN_MODEL_MX106R=0x40,
	
	DYN_MODEL_AXS1	=0x0D
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
