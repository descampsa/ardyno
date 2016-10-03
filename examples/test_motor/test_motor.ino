#include "DynamixelMotor.h"

const uint8_t id=1;
int16_t speed=256;

HardwareDynamixelInterface interface(Serial);
// Use this if you use a direction pin with 3-state buffer
//HardwareDynamixelInterface interface(Serial, 2);
// Use this if you want a sofware serial interface (communication speed will be limited)
//SoftwareDynamixelInterface interface(2,3);

DynamixelMotor motor(interface, id);

void setup()
{ 
  delay(100);
  interface.begin(1000000);
  delay(100);
  
  motor.init();
  motor.enableTorque();
  
  // reset to middle position
  motor.jointMode();
  motor.speed(speed);
  motor.goalPosition(0x1ff);
  delay(5000);
  motor.wheelMode();
}

void loop() 
{
   //change motion direction every 5 seconds
  motor.speed(speed);
  speed=-speed;
  delay(5000);
}

