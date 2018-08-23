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

#include "Toolbox.h"
#include <memory>

#define NOT_STARTET_YET -43// DEBUG


#define WHEELPERI float(0.03 * 3.1415) // Antriebsrad (Durchmesser[m] * Pi) .30mm
#define RAILPERI  float(2.048 * 3.1415)   // Kreisumfnag (Durchmesser[m] * Pi)
//#include <curses.h>

using namespace std;



CMaxonMotor::CMaxonMotor()
{
	// DEBUG
	dOverallDuration = 0;
	duration = NOT_STARTET_YET;
	start = NOT_STARTET_YET;

	ErrorCode = 0x00;
	usNodeID = 1;
	pcPortName = "USB0";
	pcDeviceName = "EPOS2";
	pcProtocolStackName = "MAXON SERIAL V2";
	pcInterfaceName = "USB";

	uiTimeout = 500;
	uiBaudrate = 1000000;

	usNominalCurrent = 1800;
	usMaxCurrent = 2400;
	usThermalConstant = 4;

	uiEncoderResolution = 1024;
	bIsInvertedPolarity = false;
	uiMaxFollowingError = 2000;

	uiMaxVelocity = 5000;
	uiMaxDecceleration = 10000;
	uiMaxAcceleration = 6000;
	//const unsigned int uiVelocityCalibrationFactor = 185; 

	iProfileVelocity_m = uiMaxVelocity; // prev. 10000
	iProfileAcceleration_m = uiMaxAcceleration; //prev. 5000
	iProfileDeceleration_m = uiMaxDecceleration; //prev. 10000


	cout << "CMaxxon constructor called success" << endl;
}

long CMaxonMotor::lgetCurrentTargetPositionInMotorData()
{
	return lCurrentTargetPositionInMotorData;
}

long CMaxonMotor::setCurrentTargetPositionInMotorData(long lTargetPositionInMotorData)
{
	lCurrentTargetPositionInMotorData = lTargetPositionInMotorData;
}

long CMaxonMotor::lConvertAngleInDegreeToMotorData(float fAngle)
{
	long lretVal = 65536.0 * (fAngle / 360.0) * (RAILPERI / WHEELPERI);
	return lretVal;
}
bool CMaxonMotor::reachedTarget(long long numberOfTimerCalls, long long numberOfMovementcalls, long long numberOfTCPCalls)//debug
{
	bool bRetVal = false;
	int targetReached;
	unsigned int uiErrorCode;
	//cout << "Check for target reach" << endl

	VCS_GetMovementState(keyHandle, usNodeID, &targetReached, &uiErrorCode);
	if (start != NOT_STARTET_YET)
	{
		duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	}

	/*while (duration < 111) // DEBUG TIME
	{
		duration = ((std::clock() - start) / (double)CLOCKS_PER_SEC) * 1000;
		usleep(1500);
	}*/

	
	if (targetReached != 0) // We reached the target position
	{
		cout << "uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu     TARGET REACHED in " << duration << "s" << endl;
		// Debug first movement rausnehmen
		if (duration < 25)
		{
			dOverallDuration += duration;
		}
		
		cout << "Overall duration of movements: " << dOverallDuration << endl;
		//cout << "number of numberOfTimerCalls" << numberOfTimerCalls << endl;
		//cout << "number of numberOfMovementcalls" << numberOfMovementcalls << endl;
		//cout << "number of numberOfTCPCalls" << numberOfTCPCalls << endl;
		duration = NOT_STARTET_YET;
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

	cout << "Closing Device!" << endl;

	if (keyHandle != 0)
	{
		VCS_CloseDevice(keyHandle, &ErrorCode);
	}
		

	VCS_CloseAllDevices(&ErrorCode);
}
void CMaxonMotor::EnableDevice()
{

	unsigned int ErrorCode = 0;
	int IsInFault = FALSE;

	if (VCS_GetFaultState(keyHandle, usNodeID, &IsInFault, &ErrorCode))
	{
		if (IsInFault && !VCS_ClearFault(keyHandle, usNodeID, &ErrorCode))
		{
			cout << "Clear fault failed! , error code=" << ErrorCode << endl;
			return;
		}

		int IsEnabled = FALSE;
		if (VCS_GetEnableState(keyHandle, usNodeID, &IsEnabled, &ErrorCode))
		{
			if (!IsEnabled && !VCS_SetEnableState(keyHandle, usNodeID, &ErrorCode))
			{
				cout << "Set enable state failed!, error code=" << ErrorCode << endl;
			}
			else
			{
				cout << "Enable succeeded!" << endl;
			}
		}
	}
	else
	{
		cout << "Get fault state failed!, error code, error code=" << ErrorCode << endl;
	}

}
void CMaxonMotor::DisableDevice()
{

	unsigned int ErrorCode = 0;
	int IsInFault = FALSE;

	if (VCS_GetFaultState(keyHandle, usNodeID, &IsInFault, &ErrorCode))
	{
		if (IsInFault && !VCS_ClearFault(keyHandle, usNodeID, &ErrorCode))
		{
			cout << "Clear fault failed!, error code=" << ErrorCode << endl;
			return;
		}

		int IsEnabled = FALSE;
		if (VCS_GetEnableState(keyHandle, usNodeID, &IsEnabled, &ErrorCode))
		{
			if (IsEnabled && !VCS_SetDisableState(keyHandle, usNodeID, &ErrorCode))
			{
				cout << "Set disable state failed!, error code=" << ErrorCode << endl;
			}
			else
			{
				cout << "Set disable state succeeded!" << endl;
			}
		}
	}
	else
	{
		//cout << "Get fault state failed!, error code=" << ErrorCode << endl;
	}
}
void CMaxonMotor::vOpenDevice()
{
	keyHandle = VCS_OpenDevice(pcDeviceName, pcProtocolStackName, pcInterfaceName, pcPortName, &ErrorCode);

	if (keyHandle == 0)
	{
		cout << "Open device failure, error code=" << ErrorCode << endl;
	}
	else
	{
		cout << "Open device success!" << endl;
	}
}
void CMaxonMotor::vSetCommunicationSettings()
{
	if (!VCS_SetProtocolStackSettings(keyHandle, uiBaudrate, uiTimeout, &ErrorCode))
	{
		cout << "Set protocol stack settings failed!, error code=" << ErrorCode << endl;
		closeDevice();
	}
}

void CMaxonMotor::activate_device()
{

	unsigned int ErrorCode = 0x00;


	cout << "Try to open the motor device .... " << endl;
	keyHandle = VCS_OpenDevice(pcDeviceName, pcProtocolStackName, pcInterfaceName, pcPortName, &ErrorCode);

	if (keyHandle == 0)
	{
		cout << "Open device failure, error code=" << ErrorCode << endl;
	}
	else
	{
		cout << "Open device success!" << endl;
	}


	if (!VCS_SetProtocolStackSettings(keyHandle, uiBaudrate, uiTimeout, &ErrorCode))
	{
		cout << "Set protocol stack settings failed!, error code=" << ErrorCode << endl;
		closeDevice();
	}

	EnableDevice();

}
void CMaxonMotor::initializeDevice() {
	        closeDevice(); // To close if opend					
	    activate_device();
	SetPosModeParameter();
}


void CMaxonMotor::vSetMainSensorType()
{
	if (!VCS_SetSensorType(keyHandle, usNodeID, ST_INC_ENCODER_3CHANNEL, &ErrorCode))
	{
		cout << "Set main sensor type failed!, error code=" << ErrorCode << endl;
		closeDevice();
	}
}
void CMaxonMotor::vSetMotorData()
{
	if (!VCS_SetDcMotorParameter(keyHandle, usNodeID, usNominalCurrent, usMaxCurrent, usThermalConstant, &ErrorCode))
	{
		cout << "Set DC Motor Data Failed with error code " << ErrorCode << endl;
		closeDevice();
	}

}
void CMaxonMotor::vSetEncoderParameter()
{
	if (!VCS_SetIncEncoderParameter(keyHandle, usNodeID, uiEncoderResolution, bIsInvertedPolarity, &ErrorCode))
	{
		cout << "Set encoder parameter failed with error code " << ErrorCode << endl;
		closeDevice();
	}
}

void CMaxonMotor::vSetMaxFollowingError()
{
	if (!VCS_SetMaxFollowingError(keyHandle, usNodeID, uiMaxFollowingError, &ErrorCode))
	{
		cout << "Max setting max Following Error Failed with error code: " << ErrorCode << endl;
		closeDevice();
	}
}

void CMaxonMotor::vSetMaxVelocity()
{
	if (!VCS_SetMaxProfileVelocity(keyHandle, usNodeID, uiMaxVelocity, &ErrorCode))
	{
		cout << "Max setting max velocity Failed with error code: " << ErrorCode << endl;
		closeDevice();
	}
}
void CMaxonMotor::vSetMaxAcceleration()
{
	if (!VCS_SetMaxAcceleration(keyHandle, usNodeID, uiMaxDecceleration, &ErrorCode))
	{
		cout << "Max setting max acceleration Failed with error code: " << ErrorCode << endl;
		closeDevice();
	}
}

void CMaxonMotor::vSetOperationMode()
{
	if (!VCS_SetOperationMode(keyHandle, usNodeID, OMD_PROFILE_POSITION_MODE, &ErrorCode))
	{
		cout << "Setting the operation mode Failed with error code: " << ErrorCode << endl;
		closeDevice();
	}
}

void CMaxonMotor::initializeDeviceNew()
{
	// To close if opend		
	closeDevice(); 
	bool bResult = false;
	// open device
	vOpenDevice();
	bResult = VCS_ResetDevice(keyHandle, usNodeID, &ErrorCode);
	if (bResult)
	{
		cout << "Reset Device OK" << ErrorCode << endl;
		// Set communication settings (USB baudrate etc)
		vSetCommunicationSettings();

		// Set main sensor type
		vSetMainSensorType();

		// Set max application velocity
		vSetMaxVelocity();

		// Set max application acceleration
		vSetMaxAcceleration();

		// set motor data
		vSetMotorData();

		// set encoder parameter
		vSetEncoderParameter();

		// set max following error
		vSetMaxFollowingError();

		// Set profile position mode 8set operation mode auf OMD_PROFILE_POSITION_MODE)
		vSetOperationMode();

		// Enable
		EnableDevice();

		// das folgende sollte in den konstruktor, ebenso die obige init funktion .... wenn alles klappt !
		setCurrentTargetPositionInMotorData(NO_MOVEMENT_IN_PROCESS);
		if (currentlyProcessedMovementData != nullptr)
		{
			currentlyProcessedMovementData->direction = NO_MOVEMENT_IN_PROCESS;
		}

		int iTempPos;
		unsigned int errorCode;
		if (VCS_ActivateProfilePositionMode(keyHandle, usNodeID, &errorCode))
		{
			int Absolute = FALSE;
			if (!Absolute)
			{
				int PositionIs = 0;
				if (VCS_GetPositionIs(keyHandle, usNodeID, &PositionIs, &errorCode)) {

				}
			}
			cout << "Activate profile position mode!" << endl;
		}
		else
		{
			cout << "Activate profile position mode failed!" << endl;
		}
		getCurrentPosition(iTempPos);
		currenTargetPos = iTempPos;
	}
	else
	{
		cout << "Reset Device Fail with code " << bResult << endl;
	}

}

void CMaxonMotor::Move(long addToCurrentPosition)
{

	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++MOTOR IS MOVING" << endl;
	int curr;
	getCurrentPosition(curr);
	//std::cout << "current position: " << curr << std::endl;
    unsigned int errorCode = 0;
	int Absolute = FALSE;
	int iImmediately = TRUE;


	if (duration == NOT_STARTET_YET) { // measurement till target reached
		cout << "StartMeasurement" << endl;
		start = std::clock();
	}
	if (!VCS_MoveToPosition(keyHandle, usNodeID, addToCurrentPosition, Absolute, iImmediately, &errorCode))
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
	std::clock_t start;
	double duration;
	start = std::clock();
    if( !VCS_GetTargetPosition(keyHandle, usNodeID, &targetPosition, &errorCode) ){
        cout << " error while getting target position , error code="<<errorCode<<endl;
    }
	/*while (duration < 111) // DEBUG TIME
	{
		duration = ((std::clock() - start) / (double)CLOCKS_PER_SEC) * 1000;
		usleep(1500);
	}*/
}

void CMaxonMotor::getCurrentPosition(int &currentPosition)
{

	unsigned int errorCode = 0;
	std::clock_t start;
	double duration;
	start = std::clock();
	if (!VCS_GetPositionIs(keyHandle, usNodeID, &currentPosition, &errorCode)) {
		//cout << " error while getting current position , error code=" << errorCode << endl;
	}
	/*while (duration < 111) // DEBUG TIME
	{
		duration = ((std::clock() - start) / (double)CLOCKS_PER_SEC) * 1000;
		usleep(1500);
	}*/
}

void CMaxonMotor::vResetTargetPositionToCurrentPosition()
{
	Move(1);
	Halt();
}

void CMaxonMotor::Halt()
{
        unsigned int ErrorCode = 0;
		std::clock_t start;
		double duration;
		start = std::clock();
        if( !VCS_HaltPositionMovement(keyHandle, usNodeID, &ErrorCode) )
        {
                cout<<"Halt position movement failed!, error code="<<ErrorCode<<endl;
        }
		/*while (duration < 111) // DEBUG TIME
		{
			duration = ((std::clock() - start) / (double)CLOCKS_PER_SEC) * 1000;
			usleep(1500);
		}*/
}
void CMaxonMotor::Error(unsigned int ErrorInfo)
{
	char* pErrorInfo;
	unsigned short iMaxStrSize=100;
	VCS_GetErrorInfo(ErrorInfo, pErrorInfo, iMaxStrSize);
}
void CMaxonMotor::ErrorNbr(unsigned char * cErrorInfo)
{
	unsigned int errorCode = 0;

	VCS_GetNbOfDeviceError(keyHandle, usNodeID, cErrorInfo, &errorCode);
}

void CMaxonMotor::SetPosModeParameter()
{

	//int iError=0;
	unsigned int uiMaxFollowingError = 2000;
	VCS_SetMaxFollowingError(keyHandle, usNodeID, uiMaxFollowingError, &ErrorCode);
	VCS_SetPositionProfile(keyHandle, usNodeID, iProfileVelocity_m, iProfileAcceleration_m, iProfileDeceleration_m, &ErrorCode);


	//cout << "SetPosModeParameter()" << endl;
	VCS_SetDcMotorParameter(keyHandle, usNodeID, usNominalCurrent, usMaxCurrent, usThermalConstant, &ErrorCode);
	cout << "1.8A" << endl;
}
void CMaxonMotor::SetCurModeParameter(int)
{
}

void CMaxonMotor::setSpeed(float speed)
{
	int iDesiredVelocity = uiVelocityCalibrationFactor * speed; // THIS IS THE CALIBRATION FOR THE MOTOR SPEED OF THE RASPII

	if (iDesiredVelocity != iProfileVelocity_m)
	{
		if (iDesiredVelocity <= uiMaxVelocity)
		{
			iProfileVelocity_m = iDesiredVelocity;
		}
		else
		{
			iDesiredVelocity = uiMaxVelocity;
		}

		VCS_SetPositionProfile(keyHandle, usNodeID, iProfileVelocity_m, iProfileAcceleration_m, iProfileDeceleration_m, &ErrorCode);
	}
}

void CMaxonMotor::GetSupply(unsigned short &  piVoltage, short int& piCurrent) {

	piVoltage = 0;
	piCurrent = 0;
	unsigned short pAnalogValue=0, InputNumber=1;
	VCS_GetCurrentIs(keyHandle, usNodeID, &piCurrent, &ErrorCode);
	VCS_GetAnalogInput(keyHandle, usNodeID, InputNumber, &pAnalogValue, &ErrorCode);
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

		if (pMovement->vecMovementqueue.front()->direction == 1) { // Dir 1 = clockwise
			liDistanceInMotorData = (lConvertAngleInDegreeToMotorData(pMovement->vecMovementqueue.front()->fAngularDistance) + liLeftOfCurrentWayToTargetPos) * -1.0 ; // Correct
		}
		if (pMovement->vecMovementqueue.front()->direction == 2) { // Dir 2 = counterclockwise
			liDistanceInMotorData = lConvertAngleInDegreeToMotorData(pMovement->vecMovementqueue.front()->fAngularDistance) + liLeftOfCurrentWayToTargetPos;
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