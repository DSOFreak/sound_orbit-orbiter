
#include "ncurses.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "wiringPi.h"
#include "cmaxonmotor.h"
//#include "gtk/gtk.h"
//#include <GL/glut.h>
//#include "freeglut_std.h"

using namespace std;

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
// we have to use BCM numbering when initializing with wiringPiSetupSys
// when choosing a different pin number please use the BCM numbering, also
// update the Property Pages - Build Events - Remote Post-Build Event command 
// which uses gpio export for setup for wiringPiSetupSys
#define	LED	17


int main(int argc, char **argv)
{




	char InterfaceName[] = "USB0";
	CMaxonMotor motor(InterfaceName, 1);
	motor.initializeDevice(); // initialize EPOS2
	
	

	long TargetPosition = -20000000;
	int CurrentPosition = 0;
	int iVoltage; 
	short iCurrent;

	motor.GetCurrentPosition(CurrentPosition); // get the current position

	motor.Move(TargetPosition); // move to the target position
	cout << "Press <Enter> to stop and quit..." << endl;
	getchar();

	motor.GetSupply(iVoltage, iCurrent);
	cout << "Motor Current : " << iCurrent << endl;


	getchar();
	motor.GetCurrentPosition(CurrentPosition); // get the current position
	cout << "Current Target: " << TargetPosition << endl;
	cout << "Current Position: " << CurrentPosition << endl;

	motor.GetCurrentPosition(CurrentPosition); // get the current position


	motor.closeDevice(); // close EPOS2


	wiringPiSetupSys();

	pinMode(LED, OUTPUT);

	while (true)
	{
		digitalWrite(LED, HIGH);  // On
		delay(500); // ms
		digitalWrite(LED, LOW);	  // Off
		delay(500);
	}
	return 0;
}