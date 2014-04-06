#include "StreamController.h"
#include "Dynamixel.h"

SerialController Controller;
DynamixelInterface Dynamixel(Serial, Controller);
DynamixelDevice device(Dynamixel, BROADCAST_ID);
bool on=true;
uint8_t buffer[256];

void setup() {
  Serial.begin(1000000);
  delay(100);
}

void loop() {
   buffer[1]=on; 
   device.write(0x19, 1, buffer+1);
   on=!on;
   delay(1000);
}

