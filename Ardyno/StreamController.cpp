
#include "Arduino.h"
//#include <avr/io.h>
#include "StreamController.h"

StreamController::StreamController()
{
	readMode();
}

StreamController::~StreamController(){}


SerialController::SerialController():
	mConfigRegister(&UCSR0B), mRxEnable(1<<RXEN0), mTxEnable(1<<TXEN0), mRxPin(0), mTxPin(1)
{}

SerialController::SerialController(volatile uint8_t *aConfigRegister, uint8_t aRxEnable, uint8_t aTxEnable, uint8_t aRxPin, uint8_t aTxPin):
	mConfigRegister(aConfigRegister), mRxEnable(aRxEnable), mTxEnable(aTxEnable), mRxPin(aRxPin), mTxPin(aTxPin)
{}

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
