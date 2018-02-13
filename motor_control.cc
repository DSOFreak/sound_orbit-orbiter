#include <stdio.h>
#include <iostream>
#include "cmaxonmotor.h"

using namespace std;

int main(int argc, char *argv[])
{
    CMaxonMotor motor("USB0",1);
    motor.initializeDevice(); // initialize EPOS2

    long TargetPosition = -200000;
    int CurrentPosition = 0;

    motor.Move(TargetPosition); // move to the target position

    cout << "Press <Enter> to stop and quit..." << endl;
    getchar();
    motor.GetCurrentPosition(CurrentPosition); // get the current position
    cout << "Current Position: " << CurrentPosition << endl;

    motor.closeDevice(); // close EPOS2

    return 0;
}
