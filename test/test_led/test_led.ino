#include <StreamController.h>
#include <Dynamixel.h>

SerialController Controller;
DynamixelInterface Dynamixel(Serial, Controller);
DynamixelDevice device(Dynamixel, BROADCAST_ID);
uint8_t on=true;

void setup() {
  Serial.begin(1000000);
  delay(100);
}

void loop() {
   device.write(0x19, on);
   on=!on;
   delay(1000);
}

