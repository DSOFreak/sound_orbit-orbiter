# Sound-Orbit Orbiter
_(Moving speaker)_

_Raspberry Pi Software for the speaker._

## Content:

1. Overview
2. Dependencies


## 1. Overview
### Resources
All Resources (images, sounds, music, etc.) have to be located in: __"/usr/lib/Orbiter/"__.

## 2. Dependencies
### FMOD
#### Description
Used for playing sound-files and generation sound.
#### Installation
1. Follow the instructions on: http://www.instructables.com/id/FMOD-Running-on-Raspberry-Pi/ (Raspberry Pi Model 3 has an armv7)
2. In Visual Studio:
	1. Add Header Files:
	Right Click on Project -> Properties -> VC++ Directories -> Add the path to the headers in Include Directories.
	2. Add Library as Linker-Flags.
	Right Click on Project -> Properties -> Linker -> Input -> Add "fmod" to the Library Dependencies

### LIRC

#### Description
Used for the IR-remote-control.
#### Installation / Instructions
To change GPIO of LIRC change the following line in "/boot/config.txt";

dtoverlay=lirc-rpi

to

dtoverlay=lirc-rpi,gpio_out_pin=17,gpio_in_pin=18

### SimpleNetwork
_https://github.com/kashimAstro/SimpleNetwork_

#### Description
Is a simple c++ wrapper for the linux network library.

To test the tcp interface you can open a console and type:

echo 001100.00100.00 | nc 127.0.0.1 1234

This will send a command to the application via the tcp interface.

The Application is now setup as tcp-server. In the future it would make more sense to change the application to a tcp-client.

#### Installation
none.
