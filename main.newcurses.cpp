


#include "cmaxonmotor.h"
#include "MvngSpkrUI.h"
#include "wiringPi.h"
#include <stdio.h>
#include <iostream>
#include <string.h>

#include <future>
#include <chrono>
#include <thread>
#include <curses.h>

//#include <pthread.h>

//#include <unistd.h>


//#include "gtk/gtk.h"
//#include <GL/glut.h>
//#include "freeglut_std.h"




using namespace std;

// "Globals accessable" in all threads
unsigned int iMode = 0;
double  dVoltage = 10.1;
unsigned short iVoltage = 0;
short iCurrent = 0;

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
// we have to use BCM numbering when initializing with wiringPiSetupSys
// when choosing a different pin number please use the BCM numbering, also
// update the Property Pages - Build Events - Remote Post-Build Event command 
// which uses gpio export for setup for wiringPiSetupSys
#define	LED	17

//cd /home/pi/projects/01_MovingSpeaker/bin/ARM/Release




CMvngSpkrUI * UserInterface;
CMaxonMotor * motor;

//void DisplayParamThread(void);




void DisplayParamThread(void) {

	int a = 1;
	a = a + a;
	//UserInterface.CurrentMenu();
}



int main(int argc, char **argv)
{
	
	
	//DisplayParamThread();
	//std::future<void> fut1 = std::async(& DisplayParamThread);
	//fut1.get();      // waits for is_prime to return
	
	//std::thread t1(DisplayParamThread);
	//t1.join();

	//int ch='a';

	//long TargetPosition = -20000000;
	int iCurrentPosition = 0;

	char InterfaceName[] = "USB0";
	//CMaxonMotor motor(InterfaceName, 1);
	motor = new CMaxonMotor(InterfaceName, 1);
	UserInterface = new CMvngSpkrUI();


	motor->initializeDevice();					// initialize EPOS2
	motor->GetCurrentPosition(iCurrentPosition);	// get the current position

	UserInterface->InitMenu();
	

	// next lines are only for testing purpose but might be used in thread loops
	auto start = std::chrono::high_resolution_clock::now();
	//usleep(2000);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;

	//move(12, 10);
	//printw("Sleep: \t%2.1f", elapsed);

	char aa;
	int bb = 0;
	do
	{
		
		//if (kbhit()) int a = 1;
		motor->GetSupply(iVoltage, iCurrent);
		dVoltage = double(3*double(iVoltage)/1000);
		UserInterface->VoltCurrShow(dVoltage, bb);
		aa = getchar();
		bb++;

	} while (aa != 'q');

	motor->closeDevice(); // close EPOS2
	delete motor;
	delete UserInterface;

	//endwin();


	//do {
	//	//move(10, 2);
	//	//printw("* The pressed key is ");

	//	//char cString[128];
	//	//strcpy(cString,"hallo");

	//	//getstr(cString);

	//	//move(9, 10);
	//	//printw("String: %s", cString);


	//	ch = getch();		/* If raw() hadn't been called
	//						* we have to press enter before it
	//						* gets to the program 		*/
	//	attron(A_BOLD);
	//	printw("%c", ch);
	//	attroff(A_BOLD);

	//	motor.GetSupply(iVoltage, iCurrent);

	//if (ch == 'g')
	//	{
	//		motor.Move(TargetPosition); // move to the target position
	//		motor.GetCurrentPosition(iCurrentPosition); // get the current position
	//		move(17, 10);
	//		printw("Position: \t%d", iCurrentPosition);

	//	}

	//	if (ch == 's')
	//	{
	//		motor.Halt();
	//		motor.GetCurrentPosition(iCurrentPosition); // get the current position
	//		move(17, 10);
	//		printw("Position: \t%d", iCurrentPosition);

	//	}

	//	move(15, 10);
	//	//dVoltage = double(iVoltage*5.6 / 15600);
	//	dVoltage = double(3*double(iVoltage)/1000);
	//	printw("I= %d mA \tU= %2.2f V", iCurrent, dVoltage);

	//} while (ch != 'q');
		

	//motor.closeDevice(); // close EPOS2



	return 0;

}