/**
 * \file DynamixelMotor.h
 * \brief Define classe for dynamixel motor
*/

#ifndef DYNAMIXEL_MOTOR_H
#define DYNAMIXEL_MOTOR_H

#include "Dynamixel.h"

/**
 * \brief Dynamixel motor control table addresses (only addresses used by all motor models)
*/
enum DynMotorAddress
{
	/** \brief Clockwise angle limit, uint16_t , writable */
	DYN_ADDRESS_CW_LIMIT		=0x06,
	/** \brief Counnter clockwise angle limit, uint16_t , writable */
	DYN_ADDRESS_CCW_LIMIT		=0x08,
	/** \brief Maximum torque, uint16_t , writable */
	DYN_ADDRESS_MAX_TORQUE		=0x0E,
	
	/** \brief Enable torque, uint8_t , writable */
	DYN_ADDRESS_ENABLE_TORQUE		=0x18,
	/** \brief LED state, uint8_t , writable */
	DYN_ADDRESS_LED					=0x19,
	/** \brief Goal position, uint16_t , writable */
	DYN_ADDRESS_GOAL_POSITION		=0x1E,
	/** \brief Goal speed, uint16_t , writable */
	DYN_ADDRESS_GOAL_SPEED		=0x20,
};

class DynamixelMotor:public DynamixelDevice
{
	public:
	
	DynamixelMotor(DynamixelInterface &aInterface, DynamixelID aId);
	
	void wheelMode();
	void jointMode(uint16_t aCWLimit=0, uint16_t aCCWLimit=0x3FF);
	
	void enableTorque(bool aTorque=true);
	void speed(int16_t aSpeed);
	void position(uint16_t aPosition);
	
	void led(uint8_t aState);
};

#endif
