

#ifndef DYNAMIXEL_INTERFACE_H
#define DYNAMIXEL_INTERFACE_H

#include "Dynamixel.h"

/**
 * \class  DynamixelInterface
 * \brief Represent a dynamixel bus
*/
class DynamixelInterface
{
	public:
	virtual void begin(unsigned long aBaud)=0;
	virtual void sendPacket(const DynamixelPacket &aPacket)=0;
	virtual void receivePacket(DynamixelPacket &aPacket, uint8_t answerSize = 0)=0;
	virtual void end()=0;
	
	void transaction(bool aExpectStatus, uint8_t answerSize = 0);
	
	//sizeof(T) must be lower than DYN_INTERNAL_BUFFER_SIZE, and in any case lower than 256
	template<class T>
	inline DynamixelStatus read(uint8_t aID, uint8_t aAddress, T& aData, uint8_t aStatusReturnLevel=2);
	template<class T>
	inline DynamixelStatus write(uint8_t aID, uint8_t aAddress, const T& aData, uint8_t aStatusReturnLevel=2);
	template<class T>
	inline DynamixelStatus regWrite(uint8_t aID, uint8_t aAddress, const T& aData, uint8_t aStatusReturnLevel=2);
	
	DynamixelStatus read(uint8_t aID, uint8_t aAddress, uint8_t aSize, uint8_t *aPtr, uint8_t aStatusReturnLevel=2);
	DynamixelStatus write(uint8_t aID, uint8_t aAddress, uint8_t aSize, const uint8_t *aPtr, uint8_t aStatusReturnLevel=2);
	DynamixelStatus regWrite(uint8_t aID, uint8_t aAddress, uint8_t aSize, const uint8_t *aPtr, uint8_t aStatusReturnLevel=2);
	DynamixelStatus syncWrite(uint8_t nID, const uint8_t *aID, uint8_t aAddress, uint8_t aSize, const uint8_t *aPtr, uint8_t aStatusReturnLevel=2);
	
	DynamixelStatus ping(uint8_t aID);
	DynamixelStatus action(uint8_t aID=BROADCAST_ID, uint8_t aStatusReturnLevel=2);
	DynamixelStatus reset(uint8_t aID, uint8_t aStatusReturnLevel=2);
	
	private:
	
	DynamixelPacket mPacket;
};

template<class T>
DynamixelStatus DynamixelInterface::read(uint8_t aID, uint8_t aAddress, T& aData, uint8_t aStatusReturnLevel)
{
	return read(aID, aAddress, uint8_t(sizeof(T)), (uint8_t*)&aData, aStatusReturnLevel);
}
template<class T>
DynamixelStatus DynamixelInterface::write(uint8_t aID, uint8_t aAddress, const T& aData, uint8_t aStatusReturnLevel)
{
	return write(aID, aAddress, uint8_t(sizeof(T)), (const uint8_t*)&aData, aStatusReturnLevel);
}
template<class T>
DynamixelStatus DynamixelInterface::regWrite(uint8_t aID, uint8_t aAddress, const T& aData, uint8_t aStatusReturnLevel)
{
	return regWrite(aID, aAddress, uint8_t(sizeof(T)), (const uint8_t*)aData, aStatusReturnLevel);
}


#if defined(ARDUINO)
#include "DynamixelInterfaceArduinoImpl.h"
#else
#error "Your platform is not supported"
#endif

#endif
