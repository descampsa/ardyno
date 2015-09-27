
#include "DynamixelInterface.h"
#include <../../libraries/SoftwareSerial/SoftwareSerial.h>

DynamixelInterface *createSerialInterface(HardwareSerial &aSerial)
{
	return new DynamixelInterfaceImpl<HardwareSerial>(aSerial);
}

DynamixelInterface *createSerialInterface(HardwareSerial &aSerial, uint8_t aDirectionPin)
{
	return new DynamixelInterfaceImpl<HardwareSerial>(aSerial, aDirectionPin);
}


namespace
{
class DynSoftwareSerial:public SoftwareSerial
{
	public:
	DynSoftwareSerial(uint8_t aRxPin, uint8_t aTxPin):
		SoftwareSerial(aRxPin, aTxPin), mTxPin(aTxPin)
	{}
	
	void enableTx(){pinMode(mTxPin, OUTPUT);}
	void disableTx(){pinMode(mTxPin, INPUT);}
	
	private:
	uint8_t mTxPin;
};
}
DynamixelInterface *createSoftSerialInterface(uint8_t aRxPin, uint8_t aTxPin)
{
	DynSoftwareSerial &serial=*new DynSoftwareSerial(aRxPin, aTxPin);
	return new DynamixelInterfaceImpl<DynSoftwareSerial>(serial, DynamixelInterfaceImpl<DynSoftwareSerial>::NO_DIR_PORT, true);
}

DynamixelInterface *createSoftSerialInterface(uint8_t aRxPin, uint8_t aTxPin, uint8_t aDirectionPin)
{
	DynSoftwareSerial &serial=*new DynSoftwareSerial(aRxPin, aTxPin);
	return new DynamixelInterfaceImpl<DynSoftwareSerial>(serial, aDirectionPin, true);
}

namespace {
//dirty trick to access protected member
class HardwareSerialAccess:public HardwareSerial
{
	public:
	volatile uint8_t * const ucsrb(){return _ucsrb;}
};
}

template<>
void setReadMode<HardwareSerial>(HardwareSerial &aStream)
{
	HardwareSerialAccess &stream=reinterpret_cast<HardwareSerialAccess&>(aStream);
	*(stream.ucsrb()) &= !_BV(TXEN0);
	*(stream.ucsrb()) |= _BV(RXEN0);
	*(stream.ucsrb()) |= _BV(RXCIE0);
}

template<>
void setWriteMode<HardwareSerial>(HardwareSerial &aStream)
{
	HardwareSerialAccess &stream=reinterpret_cast<HardwareSerialAccess&>(aStream);
	*(stream.ucsrb()) &= !_BV(RXEN0);
	*(stream.ucsrb()) |= _BV(TXEN0);
}


template<>
void setReadMode<DynSoftwareSerial>(DynSoftwareSerial &aStream)
{
	aStream.disableTx();
	aStream.listen();
}

template<>
void setWriteMode<DynSoftwareSerial>(DynSoftwareSerial &aStream)
{
	aStream.stopListening();
	aStream.enableTx();
}


