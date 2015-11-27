/**
 * \file Dynamixel.h
 * \brief Define classes for dynamixel protocol
*/

#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include <Arduino.h>

/** \brief Type of dynamixel device ID */
typedef uint8_t DynamixelID;
/** \brief Type of dynamixel status code */
typedef uint8_t DynamixelStatus;
/** \brief Type of dynamixel instructions */
typedef uint8_t DynamixelInstruction;

/** \brief ID for broadcast */
#define BROADCAST_ID 0xFE


/**
 * \brief Dynamixel intruction values
*/
enum DynInstruction
{
	DYN_PING		= 0x01,
	DYN_READ		= 0x02,
	DYN_WRITE		= 0x03,
	DYN_REG_WRITE	= 0x04,
	DYN_ACTION		= 0x05,
	DYN_RESET		= 0x06,
	DYN_SYNC_WRITE	= 0x07
};

/**
 * \brief Dynamixel status values
 *
 * How to interpret status value :
 *
 * If (status&DYN_STATUS_COM_ERROR)==0 , the value is the 
 * the status returned by the motor, describing its internal
 * error.
 * If (status&DYN_STATUS_COM_ERROR)==1, there was an error during
 * communication, and the value describe that error.
 *
 * DYN_STATUS_CHECKSUM_ERROR may appear in both cases, in the first
 * case, it means there was an error in the checksum of the 
 * instruction packet, in second case in the response packet.
 *
 *
*/
enum DynStatus
{
	DYN_STATUS_OK					= 0,
	
	DYN_STATUS_INPUT_VOLTAGE_ERROR	= 1,
	DYN_STATUS_ANGLE_LIMIT_ERROR	= 2,
	DYN_STATUS_OVERHEATING_ERROR	= 4,
	DYN_STATUS_RANGE_ERROR			= 8,
	DYN_STATUS_CHECKSUM_ERROR		= 16,
	DYN_STATUS_OVERLOAD_ERROR		= 32,
	DYN_STATUS_INSTRUCTION_ERROR	= 64,
	
	DYN_STATUS_TIMEOUT				=1,
	
	DYN_STATUS_COM_ERROR			= 128,
	DYN_STATUS_INTERNAL_ERROR		= 255
};


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

#ifndef DYN_INTERNAL_BUFFER_SIZE
#define DYN_INTERNAL_BUFFER_SIZE 32
#endif
/**
 * \class  DynamixelInterface
 * \brief Represent a dynamixel bus
*/
class DynamixelInterface
{
	public:
	virtual void begin(unsigned long aBaud)=0;
	virtual void sendPacket(const DynamixelPacket &aPacket)=0;
	virtual void receivePacket(DynamixelPacket &aPacket)=0;
	
	void transaction(uint8_t aID, uint8_t aInstruction, uint8_t aLenght, uint8_t *aData, bool aExpectStatus);
	
	//sizeof(T) must be lower than DYN_INTERNAL_BUFFER_SIZE, and in any case lower than 256
	template<class T>
	inline DynamixelStatus read(uint8_t aID, uint8_t aAddress, T& aData, uint8_t aStatusReturnLevel=2);
	template<class T>
	inline DynamixelStatus write(uint8_t aID, uint8_t aAddress, const T& aData, uint8_t aStatusReturnLevel=2);
	template<class T>
	inline DynamixelStatus regWrite(uint8_t aID, uint8_t aAddress, const T& aData, uint8_t aStatusReturnLevel=2);
	
	// warning : array pointed by aPtr must be at least two bytes long
	DynamixelStatus read(uint8_t aID, uint8_t aAddress, uint8_t aSize, uint8_t *aPtr, uint8_t aStatusReturnLevel=2);
	// warning : one byte must be allocated before aPtr
	DynamixelStatus write(uint8_t aID, uint8_t aAddress, uint8_t aSize, uint8_t *aPtr, uint8_t aStatusReturnLevel=2);
	// warning : one byte must be allocated before aPtr
	DynamixelStatus regWrite(uint8_t aID, uint8_t aAddress, uint8_t aSize, uint8_t *aPtr, uint8_t aStatusReturnLevel=2);
	
	DynamixelStatus ping(uint8_t aID);
	DynamixelStatus action(uint8_t aID=BROADCAST_ID, uint8_t aStatusReturnLevel=2);
	DynamixelStatus reset(uint8_t aID, uint8_t aStatusReturnLevel=2);
	
	private:
	
	DynamixelPacket mPacket;
	
	uint8_t mInternalBuffer[DYN_INTERNAL_BUFFER_SIZE];
};

/** \brief Create dynamixel interface from hardware uart */
DynamixelInterface *createSerialInterface(HardwareSerial &aSerial);
/** \brief Create dynamixel interface from hardware uart with direction port connected to a 3-state buffer */
DynamixelInterface *createSerialInterface(HardwareSerial &aSerial, uint8_t aDirectionPin);
/** \brief Create dynamixel interface from software uart (need SoftwareSerial) */
DynamixelInterface *createSoftSerialInterface(uint8_t aRxPin, uint8_t aTxPin);
/** \brief Create dynamixel interface from software uart with direction port connected to a 3-state buffer (need SoftwareSerial) */
DynamixelInterface *createSoftSerialInterface(uint8_t aRxPin, uint8_t aTxPin, uint8_t aDirectionPin);


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

class DynamixelDevice
{
	public:
	
	DynamixelDevice(DynamixelInterface &aInterface, DynamixelID aId);
	
	DynamixelStatus init();
	
	DynamixelStatus status()
	{
		return mStatus;
	}
	
	DynamixelID id()
	{
		return mID;
	}
	
	uint8_t statusReturnLevel();
	void statusReturnLevel(uint8_t aSRL);
	
	uint16_t model();
	uint8_t firmware();
	
	void communicationSpeed(uint32_t aSpeed);
	
	
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


template<class T>
DynamixelStatus DynamixelInterface::read(uint8_t aID, uint8_t aAddress, T& aData, uint8_t aStatusReturnLevel)
{
	read(aID, aAddress, uint8_t(sizeof(T)), mInternalBuffer, aStatusReturnLevel);
	memcpy(&aData, mInternalBuffer, sizeof(T));
}
template<class T>
DynamixelStatus DynamixelInterface::write(uint8_t aID, uint8_t aAddress, const T& aData, uint8_t aStatusReturnLevel)
{
	memcpy(mInternalBuffer+1, &aData, sizeof(T));
	write(aID, aAddress, uint8_t(sizeof(T)), mInternalBuffer+1, aStatusReturnLevel);
}
template<class T>
DynamixelStatus DynamixelInterface::regWrite(uint8_t aID, uint8_t aAddress, const T& aData, uint8_t aStatusReturnLevel)
{
	memcpy(mInternalBuffer+1, &aData, sizeof(T));
	regWrite(aID, aAddress, uint8_t(sizeof(T)), mInternalBuffer+1, aStatusReturnLevel);
}

#endif
