
#include "DynamixelInterface.h"
#include <SoftwareSerial.h>

#if __AVR__
// define TXEN, RXEN and RXCIE
#if !defined(TXEN)
#if defined(TXEN0)
#define TXEN TXEN0
#define RXEN RXEN0
#define RXCIE RXCIE0
#elif defined(TXEN1) // Some devices have uart1 but no uart0 (leonardo)
#define TXEN TXEN1
#define RXEN RXEN1
#define RXCIE RXCIE1
#endif
#endif
#endif //__AVR__

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

#if __AVR__
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
	*(stream.ucsrb()) &= ~_BV(TXEN);
	*(stream.ucsrb()) |= _BV(RXEN);
	*(stream.ucsrb()) |= _BV(RXCIE);
}

template<>
void setWriteMode<HardwareSerial>(HardwareSerial &aStream)
{
	HardwareSerialAccess &stream=reinterpret_cast<HardwareSerialAccess&>(aStream);
	*(stream.ucsrb()) &= ~_BV(RXEN);
	*(stream.ucsrb()) |= _BV(RXCIE);
	*(stream.ucsrb()) |= _BV(TXEN);
}

#elif __arc__

//Arduino 101 specific code

template<>
void setReadMode<HardwareSerial>(HardwareSerial &aStream)
{
	//enable pull up to avoid noise on the line
	pinMode(1, INPUT);
	digitalWrite(1, HIGH);
	// disconnect UART TX and connect UART RX
	SET_PIN_MODE(16, GPIO_MUX_MODE);
	SET_PIN_MODE(17, UART_MUX_MODE);
}

template<>
void setWriteMode<HardwareSerial>(HardwareSerial &aStream)
{
	// disconnect UART RX and connect UART TX
	SET_PIN_MODE(17, GPIO_MUX_MODE);
	SET_PIN_MODE(16, UART_MUX_MODE);
}

#endif


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


