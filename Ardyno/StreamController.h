/**
 * \file StreamController.h
 * \brief StreamController classes are responsible of switching underlying connections (generally bidirectionnal) between receive and send modes
 *
 * \todo Add StreamController class for SoftSerial
*/

#ifndef STREAMCONTROLLER_H
#define STREAMCONTROLLER_H

typedef unsigned char uint8_t;

/**
 * \class StreamController
 * \brief Base class, define interface for StreamController implementations
*/
class StreamController
{
	public:
	
	/** \brief Constructor */
	StreamController();
	/** \brief Destructor */
	virtual ~StreamController();
	
	/**
	 * \brief Switch stream to read (receive)) mode 
	 * Default implementation do nothing.
	*/
	virtual void readMode(){}
	/**
	 * \brief Switch stream to write (send) mode 
	 * Default implementation do nothing.
	*/
	virtual void writeMode(){}
};

/**
 * \class SerialController
 * \brief StreamController implementation for HardwareSerial streams
*/
class SerialController:public StreamController
{
	public:
	
	/**
	 * \brief Constructor
	 * \param aSerial[in] : index of the Serial connection used
	 * Use 0 (default) for Serial, 1 for Serial1, 2 for Serial2 or 3 for Serial3
	*/
	SerialController(uint8_t aSerial=0);
	/**
	 * \brief Destructor
	 * \todo The state of connection after destruction should be defined 
	*/
	virtual ~SerialController();
	
	/** \brief Switch Serial connection to read (receive) mode  */
	virtual void readMode();
	/** \brief Switch Serial connection to write (send) mode  */
	virtual void writeMode();
	
	private:
	
	volatile uint8_t *mConfigRegister;
	uint8_t mRxEnable, mTxEnable;
	uint8_t mRxPin, mTxPin;
};

#endif
