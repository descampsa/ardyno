ardyno
======

Arduino library for dynamixel servos

This library allows you to control dynamixel devices with an arduino.
It uses the hardware uart of arduino to communicate at 1000000 Bd.

See the test_led program for an example of how to use it.

It has only been tested with Arduino Uno (should work with other versions with a few adaptations). Also see warning below.
Obviously, only TTL devices may be controlled without additionnal hardware, for RS485 devices, an adaptator would be needed.

#Installation

Copy the Ardyno directory in your sketchbook/libraries directory.

#Usage

You can use a 9V or 12V power supply to power the arduino and motors.
Connect GND and VIN pins of the adruino and dynamixel device, and tx and rx pins of the arduino to the data pin of your dynamixel device.

Warning : 
The dynamixel uses a half duplex connection (RS485-like).
For now, the switch between rx and tx mode is done by software, by disabling rx pin when is in tx mode, and vice versa.
This is probably not optimal, but it allows to use it without additional hardware and does not seem to cause any problem.
Use it at you own risk.
