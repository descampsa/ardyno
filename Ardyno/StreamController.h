
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
	
	SerialController(uint8_t aUART=0);
	virtual ~SerialController();
	
	virtual void readMode();
	virtual void writeMode();
	
	private:
	
	volatile uint8_t *mConfigRegister;
	uint8_t mRxEnable, mTxEnable;
	uint8_t mRxPin, mTxPin;
};

#endif
