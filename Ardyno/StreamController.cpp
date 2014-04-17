/**
 * \file StreamController.cpp
*/


#include "Arduino.h"
//#include <avr/io.h>
#include "StreamController.h"

StreamController::StreamController()
{
	readMode();
}

StreamController::~StreamController(){}


SerialController::SerialController(uint8_t aSerial)
{
#if defined(HAVE_HWSERIAL0)
	if(aSerial==0)
	{
		mConfigRegister = &UCSR0B;
		mRxEnable = 1<<RXEN0;
		mTxEnable = 1<<TXEN0;
		mRxPin = 0;
		mTxPin = 1;
	}
#endif
#if defined(HAVE_HWSERIAL1)
	if(aSerial==1)
	{
		mConfigRegister = &UCSR1B;
		mRxEnable = 1<<RXEN1;
		mTxEnable = 1<<TXEN1;
		mRxPin = 19;
		mTxPin = 18;
	}
#endif
#if defined(HAVE_HWSERIAL2)
	if(aSerial==2)
	{
		mConfigRegister = &UCSR2B;
		mRxEnable = 1<<RXEN2;
		mTxEnable = 1<<TXEN2;
		mRxPin = 17;
		mTxPin = 16;
	}
#endif
#if defined(HAVE_HWSERIAL3)
	if(aSerial==3)
	{
		mConfigRegister = &UCSR3B;
		mRxEnable = 1<<RXEN3;
		mTxEnable = 1<<TXEN3;
		mRxPin = 15;
		mTxPin = 14;
	}
#endif
}

SerialController::~SerialController(){}

void SerialController::readMode()
{
	*mConfigRegister &= ~mTxEnable;
	pinMode(mTxPin, INPUT);
	*mConfigRegister |= mRxEnable;
}

void SerialController::writeMode()
{
	*mConfigRegister &= ~mRxEnable;
	pinMode(mRxPin, INPUT);
	*mConfigRegister |= mTxEnable;
}
