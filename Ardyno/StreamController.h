
#ifndef STREAMCONTROLLER_H
#define STREAMCONTROLLER_H

typedef unsigned char uint8_t;

class StreamController
{
	public:
	
	StreamController();
	virtual ~StreamController();
	
	virtual void readMode(){}
	virtual void writeMode(){}
};

class SerialController:public StreamController
{
	public:
	
	SerialController();
	SerialController(volatile uint8_t *aConfigRegister, uint8_t mRxEnable, uint8_t mTxEnable, uint8_t mRxPin, uint8_t mTxPin);
	virtual ~SerialController();
	
	virtual void readMode();
	virtual void writeMode();
	
	private:
	
	volatile uint8_t *mConfigRegister;
	uint8_t mRxEnable, mTxEnable;
	uint8_t mRxPin, mTxPin;
};

#endif
