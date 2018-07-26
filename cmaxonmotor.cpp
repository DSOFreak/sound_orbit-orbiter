#include "cmaxonmotor.h"
#include "Movement.h"
#include <string.h>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <lirc/lirc_client.h> //usleep
#include "RaspiConfig.h"
#include <fstream>      // std::fstream
#include <fcntl.h>

#define WHEELPERI float(0.03 * 3.1415) // Antriebsrad (Durchmesser[m] * Pi) .30mm
#define RAILPERI  float(2.048 * 3.1415)   // Kreisumfnag (Durchmesser[m] * Pi)
//#include <curses.h>

using namespace std;



CMaxonMotor::CMaxonMotor()
{
    strcpy(PortName, "USB0");
    ErrorCode = 0x00;
    nodeID = 1;
	setCurrentTargetPositionInMotorData(NO_MOVEMENT_IN_PROCESS);
	currentlyProcessedMovementData->direction = NO_MOVEMENT_IN_PROCESS;
	int iTempPos;
	unsigned int errorCode;
	if (VCS_ActivateProfilePositionMode(keyHandle, nodeID, &errorCode))
	{
		int Absolute = FALSE;
		if (!Absolute)
		{
			int PositionIs = 0;
			if (VCS_GetPositionIs(keyHandle, nodeID, &PositionIs, &errorCode)) {

			}
		}
	}
	else
	{
		cout << "Activate profile position mode failed!" << endl;
	}

	getCurrentPosition(iTempPos);
	currenTargetPos = iTempPos;
}
CMaxonMotor::CMaxonMotor(char* portNamestr, unsigned short input_node_Id)
{
    PortName = portNamestr;
    ErrorCode = 0x00;
    nodeID = input_node_Id;
}
long CMaxonMotor::lgetCurrentTargetPositionInMotorData()
{
	return lCurrentTargetPositionInMotorData;
}

long CMaxonMotor::setCurrentTargetPositionInMotorData(long lTargetPositionInMotorData)
{
	lCurrentTargetPositionInMotorData = lTargetPositionInMotorData;
}

long CMaxonMotor::lConvertAngleInDegreeToMotorData(int iAngle)
{
	long lretVal = 65536 * (float(iAngle) / 360.0) * (RAILPERI / WHEELPERI);
	return lretVal;
}
bool CMaxonMotor::reachedTarget()
{
	bool bRetVal = false;
	int targetReached;
	unsigned int uiErrorCode;
	/*cout << "Check for target reach" << endl;
	std::clock_t start;
	double duration;
	start = std::clock();
	float load1;
	int errorl = GetCPULoad(load1);*/
	VCS_GetMovementState(keyHandle, nodeID, &targetReached, &uiErrorCode);
	/*float load;
	int error = GetCPULoad(load);
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

	std::cout << "Duration for Check for target reach : " << duration << "with cpu Loads: "<< load << " " << load1 << "errorcode: " << error << '\n';*/

	
	if (targetReached != 0) // We reached the target position
	{
		
		bRetVal = true;
		setCurrentTargetPositionInMotorData(NO_MOVEMENT_IN_PROCESS);
		if (currentlyProcessedMovementData != nullptr)
		{
			currentlyProcessedMovementData->direction = NO_MOVEMENT_IN_PROCESS;
		}
		
		
	}
	else
	{
		//cout << "~~~~~~~~~~~~~~~~We did NOT reached the target position" << endl;
		bRetVal = false;
	}
	if (uiErrorCode != 0)
	{
		cout << "ERROR IN ReachedTarget() number: " << uiErrorCode << endl;
	}
	return bRetVal;
}


void CMaxonMotor::closeDevice()
{
    DisableDevice();

    unsigned int ErrorCode = 0;

	//move(17, 10);

    if(keyHandle != 0)
        VCS_CloseDevice(keyHandle, &ErrorCode);

    VCS_CloseAllDevices(&ErrorCode);
	//move(16, 10);
	//printw("Status: \t%x", ErrorCode);
}
void CMaxonMotor::EnableDevice()
{

    unsigned int ErrorCode = 0;
    int IsInFault = FALSE;

    if( VCS_GetFaultState(keyHandle, nodeID, &IsInFault, &ErrorCode) )
    {
        if( IsInFault && !VCS_ClearFault(keyHandle, nodeID, &ErrorCode) )
        {
            cout << "Clear fault failed! , error code="<<ErrorCode<<endl;
            return;
        }

        int IsEnabled = FALSE;
        if( VCS_GetEnableState(keyHandle, nodeID, &IsEnabled, &ErrorCode) )
        {
            if( !IsEnabled && !VCS_SetEnableState(keyHandle, nodeID, &ErrorCode) )
            {
                cout << "Set enable state failed!, error code="<<ErrorCode<<endl;
            }
            else
            {
				//move(16, 10);
				//printw("Status: \t%x", ErrorCode);
				
            }
        }
    }
    else
    {
		//move(16, 10);
		//printw("Status: %x", ErrorCode);

    }

}
void CMaxonMotor::DisableDevice()
{

    unsigned int ErrorCode = 0;
    int IsInFault = FALSE;

    if( VCS_GetFaultState(keyHandle, nodeID, &IsInFault, &ErrorCode) )
    {
        if( IsInFault && !VCS_ClearFault(keyHandle, nodeID, &ErrorCode) )
        {
 			//move(16, 10);
			//printw("Status: \tError 0x%x", ErrorCode);
			return;
        }

        int IsEnabled = FALSE;
        if( VCS_GetEnableState(keyHandle, nodeID, &IsEnabled, &ErrorCode) )
        {
            if( IsEnabled && !VCS_SetDisableState(keyHandle, nodeID, &ErrorCode) )
            {
				//move(16, 10);
				//printw("Status: \tError 0x%x", ErrorCode);
			}
            else
            {
				//move(16, 10);
				//printw("Status: \t%x", ErrorCode);
			}
        }
    }
    else
    {
		//move(16, 10);
		//printw("Status: Error 0x%x", ErrorCode);
	}
}
void CMaxonMotor::activate_device()
{
    // Configuring EPOS for analog motor control
    char DeviceName[]="EPOS2";
    char ProtocolStackName[] = "MAXON SERIAL V2";
    char InterfaceName[] = "USB";
    unsigned int ErrorCode = 0x00;
    unsigned long timeout_ = 500;
    unsigned long baudrate_ = 1000000;


    keyHandle = VCS_OpenDevice(DeviceName,ProtocolStackName,InterfaceName,PortName,&ErrorCode);

    if( keyHandle == 0 )
    {
        cout<<"Open device failure, error code="<<ErrorCode<<endl;
    }
    else
    {
        cout<<"Open device success!"<<endl;
    }


    if( !VCS_SetProtocolStackSettings(keyHandle, baudrate_, timeout_, &ErrorCode) )
    {
        cout<<"Set protocol stack settings failed!, error code="<<ErrorCode<<endl;
        closeDevice();
    }

    EnableDevice();
	VCS_ResetPositionMarkerCounter(keyHandle, nodeID, &ErrorCode);

}
void CMaxonMotor::initializeDevice() {
	        closeDevice(); // To close if opend					
	    activate_device();
	SetPosModeParameter();
}

void CMaxonMotor::Move(long addToCurrentPosition)
{

	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++MOTOR IS MOVING" << endl;
	int curr;
	getCurrentPosition(curr);
	//std::cout << "current position: " << curr << std::endl;
    unsigned int errorCode = 0;
	int Absolute = FALSE;
	int Immediately = TRUE;

	if (!VCS_MoveToPosition(keyHandle, nodeID, addToCurrentPosition, Absolute, Immediately, &errorCode))
	{
		cout << "Move to position failed!, error code=" << errorCode << endl;
	}
	std::cout << "apply change of: " << addToCurrentPosition << " to current position " << curr << std::endl;
	std::cout << "New position should be: " << addToCurrentPosition + curr << std::endl;
	currenTargetPos = addToCurrentPosition + curr;
	
}

void CMaxonMotor::getCurrentlyProcessedTargetPosition(long int &targetPosition)
{

    unsigned int errorCode = 0;

    if( !VCS_GetTargetPosition(keyHandle, nodeID, &targetPosition, &errorCode) ){
        cout << " error while getting target position , error code="<<errorCode<<endl;
    }

}

void CMaxonMotor::getCurrentPosition(int &currentPosition)
{

	unsigned int errorCode = 0;

	if (!VCS_GetPositionIs(keyHandle, nodeID, &currentPosition, &errorCode)) {
		//cout << " error while getting current position , error code=" << errorCode << endl;
	}

}

void CMaxonMotor::Halt()
{
        unsigned int ErrorCode = 0;

        if( !VCS_HaltPositionMovement(keyHandle, nodeID, &ErrorCode) )
        {
                cout<<"Halt position movement failed!, error code="<<ErrorCode<<endl;
        }
}
void CMaxonMotor::Error(unsigned int ErrorInfo)
{
	char* pErrorInfo;
	unsigned short iMaxStrSize=10;
	VCS_GetErrorInfo(ErrorInfo, pErrorInfo, iMaxStrSize);
}
void CMaxonMotor::ErrorNbr(unsigned char * cErrorInfo)
{
	unsigned int errorCode = 0;

	VCS_GetNbOfDeviceError(keyHandle, nodeID, cErrorInfo, &errorCode);
}

void CMaxonMotor::SetPosModeParameter()
{
	unsigned int uiMaxFollowingError = 20000;
	unsigned int iProfileVelocity, iProfileAcceleration, iProfileDeceleration;
	unsigned short iNominalCurrent, iMaxOutputCurrent, iThermalTimeConstant;
	//int iError=0;

	VCS_SetMaxFollowingError(keyHandle, nodeID, uiMaxFollowingError, &ErrorCode);
	VCS_GetPositionProfile(keyHandle, nodeID, &iProfileVelocity, &iProfileAcceleration, &iProfileDeceleration, &ErrorCode);
	iProfileVelocity = 10000;
	iProfileAcceleration = 5000; //prev. 5000
	iProfileDeceleration = 10000; //prev. 10000
	VCS_SetPositionProfile(keyHandle, nodeID, iProfileVelocity, iProfileAcceleration, iProfileDeceleration, &ErrorCode);


	VCS_GetDcMotorParameter(keyHandle, nodeID, &iNominalCurrent, &iMaxOutputCurrent, &iThermalTimeConstant, &ErrorCode);
	VCS_SetDcMotorParameter(keyHandle, nodeID, 1800, 2000, 40, &ErrorCode);
	cout << "1.8A" << endl;
}
void CMaxonMotor::SetCurModeParameter(int)
{
}

void CMaxonMotor::setSpeed(float speed)
{
	unsigned int uiMaxFollowingError = 20000;
	unsigned int iProfileVelocity, iProfileAcceleration, iProfileDeceleration;
	unsigned short iNominalCurrent, iMaxOutputCurrent, iThermalTimeConstant;
	//int iError=0;

	unsigned int errorCode = 0;

	
	VCS_GetPositionProfile(keyHandle, nodeID, &iProfileVelocity, &iProfileAcceleration, &iProfileDeceleration, &ErrorCode);
	iProfileVelocity = 183 * speed; // THIS IS THE CALIBRATION FOR THE MOTOR SPEED OF THE RASPI
	VCS_SetPositionProfile(keyHandle, nodeID, iProfileVelocity, iProfileAcceleration, iProfileDeceleration, &ErrorCode);
	VCS_ActivateProfilePositionMode(keyHandle, nodeID, &errorCode);

	
}

void CMaxonMotor::GetSupply(unsigned short &  piVoltage, short int& piCurrent) {

	piVoltage = 0;
	piCurrent = 0;
	unsigned short pAnalogValue=0, InputNumber=1;
	VCS_GetCurrentIs(keyHandle, nodeID, &piCurrent, &ErrorCode);
	VCS_GetAnalogInput(keyHandle, nodeID, InputNumber, &pAnalogValue, &ErrorCode);
	piVoltage = pAnalogValue;
}



int CMaxonMotor::GetCPULoad(float &load) {
	int FileHandler;
	char FileBuffer[1024];

	FileHandler = open("/proc/loadavg", O_RDONLY);
	if (FileHandler < 0) {
		return -1;
	}
	read(FileHandler, FileBuffer, sizeof(FileBuffer) - 1);
	sscanf(FileBuffer, "%f", &load);
	close(FileHandler);
}


bool CMaxonMotor::bTryToAddMovementDataToCurrentMovement()
{
	shared_ptr<Movement> pMovement = Movement::getInstance();
	if (pMovement->vecMovementqueue.empty() || (currentlyProcessedMovementData == nullptr) || (pMovement->vecMovementqueue.front()->direction == 0))
	{
		return false;
	}
	// movement pending 
	if ((currentlyProcessedMovementData->direction == pMovement->vecMovementqueue.front()->direction) && (currentlyProcessedMovementData->speed == pMovement->vecMovementqueue.front()->speed))
	{// we have a match
		// add the movement to the current motor movement (means, call it with an updated targetPosition)
		long int liCurrentlyProcessedAddToPosition;
		getCurrentlyProcessedTargetPosition(liCurrentlyProcessedAddToPosition);
		int iCurrentPosition;
		getCurrentPosition(iCurrentPosition);
		cout << "iCurrentPosition " << iCurrentPosition << endl;
		cout << "iCurrentTargetPosition " << currenTargetPos << endl;
		cout << "liCurrentlyProcessedAddToPosition " << liCurrentlyProcessedAddToPosition << endl;
		long int liLeftOfCurrentWayToTargetPos = abs(currenTargetPos - iCurrentPosition);
		cout << "liLeftOfCurrentWayToTargetPos " << liLeftOfCurrentWayToTargetPos << endl;



		long int liDistanceInMotorData;
		if (RaspiConfig::ownIndex == 1)
		{
			if (pMovement->vecMovementqueue.front()->direction == 1) { // Dir 1 = clockwise
				liDistanceInMotorData = lConvertAngleInDegreeToMotorData(pMovement->vecMovementqueue.front()->angularDistance) + liLeftOfCurrentWayToTargetPos; // Correct
			}
			if (pMovement->vecMovementqueue.front()->direction == 2) { // Dir 2 = counterclockwise
				liDistanceInMotorData = (lConvertAngleInDegreeToMotorData(pMovement->vecMovementqueue.front()->angularDistance)  + liLeftOfCurrentWayToTargetPos) * -1;
			}
		}
		else
		{
			if (pMovement->vecMovementqueue.front()->direction == 1) { // Dir 1 = clockwise
				liDistanceInMotorData = (lConvertAngleInDegreeToMotorData(pMovement->vecMovementqueue.front()->angularDistance) + liLeftOfCurrentWayToTargetPos) * -1 ; // Correct
			}
			if (pMovement->vecMovementqueue.front()->direction == 2) { // Dir 2 = counterclockwise
				liDistanceInMotorData = lConvertAngleInDegreeToMotorData(pMovement->vecMovementqueue.front()->angularDistance) + liLeftOfCurrentWayToTargetPos;
			}
		}

		//Update the target position


		cout << "liDistanceInMotorData " << liDistanceInMotorData << endl;
		setCurrentTargetPositionInMotorData(liDistanceInMotorData);
		Move(lgetCurrentTargetPositionInMotorData());

		// delete the first element of the movement queue
		pMovement->vecMovementqueue.erase(pMovement->vecMovementqueue.begin());
		return true;
	}
	else
	{
		return false;
	}
}