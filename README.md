ardyno
======

Arduino library for dynamixel servos

This library allows you to control dynamixel devices with an arduino.
It uses the hardware uart of arduino to communicate at 1000000 Bd.
It should also be possible to use the softserial library,  but probably not at 1000000 Bd.

See the test_led program for an example of how to use it.

It has only been tested with Arduino Uno (should work with other versions with a few adaptations), and with no more than two devices connected on the bus. Also see warning below.

#Installation

Copy the Ardyno directory in your sketchbook/libraries directory.

#Usage

Simply connect tx and rx pins of the arduino to the data pin of your dynamixel device.

Warning : 
The dynamixel uses a half duplex connection. 
For now, the switch between rx and tx mode is done by software, by disabling rx pin when is in tx mode, and vice versa.
This is not safe and far from optimal, but it allows to use it without additional hardware. Use only at you own risk and if you understand what you are doing.
A proper version, with a tristate buffer interface is planned soon.