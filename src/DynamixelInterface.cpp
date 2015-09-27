
#include "DynamixelInterface.h"


DynamixelInterface *createSerialInterface(HardwareSerial &aSerial)
{
	return new DynamixelInterfaceImpl<HardwareSerial>(aSerial);
}

DynamixelInterface *createSerialInterface(HardwareSerial &aSerial, uint8_t aDirectionPort)
{
	return new DynamixelInterfaceImpl<HardwareSerial>(aSerial, aDirectionPort);
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
