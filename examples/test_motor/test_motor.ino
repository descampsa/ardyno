
#include "DynamixelMotor.h"

const uint8_t id=2;
int16_t speed=256;
DynamixelInterface &interface=*createSerialInterface(Serial);
// Use this if you use a direction pin with 3-state buffer
//DynamixelInterface &interface=*createSerialInterface(Serial, 2);
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
  motor.position(0x1ff);
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

