#include "cmaxonmotor.h"

#include <string.h>
#include <iostream>
//#include <curses.h>

using namespace std;

CMaxonMotor::CMaxonMotor()
{
    strcpy(PortName, "USB0");
    ErrorCode = 0x00;
    nodeID = 1;
}
CMaxonMotor::CMaxonMotor(char* portNamestr, unsigned short input_node_Id)
{
    PortName = portNamestr;
    ErrorCode = 0x00;
    nodeID = input_node_Id;
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

}
void CMaxonMotor::initializeDevice() {
	        closeDevice(); // To close if opend					
	    activate_device();
	SetPosModeParameter();
}

void CMaxonMotor::Move(long TargetPosition)
{

    unsigned int errorCode = 0;

    if( VCS_ActivateProfilePositionMode(keyHandle, nodeID, &errorCode) )
    {
        int Absolute = TRUE; // FALSE;
        int Immediately = TRUE;

        if( !Absolute )
        {
            int PositionIs = 0;
			if (VCS_GetPositionIs(keyHandle, nodeID, &PositionIs, &errorCode)) {

			}
        }

        if( !VCS_MoveToPosition(keyHandle, nodeID, TargetPosition, Absolute, Immediately, &errorCode) )
        {
            cout << "Move to position failed!, error code="<<errorCode<<endl;

        }
    }
    else
    {
        cout << "Activate profile position mode failed!" << endl;
    }
}
void CMaxonMotor::GetCurrentPosition(int& CurrentPosition)
{

    unsigned int errorCode = 0;

    if( !VCS_GetPositionIs(keyHandle, nodeID, &CurrentPosition, &errorCode) ){
        cout << " error while getting current position , error code="<<errorCode<<endl;
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
	unsigned int iProfileVelocity = 0, iProfileAcceleration=0, iProfileDeceleration=0;
	unsigned short iNominalCurrent = 0, iMaxOutputCurrent=0, iThermalTimeConstant=0;
	//int iError=0;

	VCS_SetMaxFollowingError(keyHandle, nodeID, uiMaxFollowingError, &ErrorCode);
	VCS_GetPositionProfile(keyHandle, nodeID, &iProfileVelocity, &iProfileAcceleration, &iProfileDeceleration, &ErrorCode);
	iProfileVelocity = 10000;
	iProfileAcceleration = 1000; 
	iProfileDeceleration = 2000;
	VCS_SetPositionProfile(keyHandle, nodeID, iProfileVelocity, iProfileAcceleration, iProfileDeceleration, &ErrorCode);


	VCS_GetDcMotorParameter(keyHandle, nodeID, &iNominalCurrent, &iMaxOutputCurrent, &iThermalTimeConstant, &ErrorCode);
	VCS_SetDcMotorParameter(keyHandle, nodeID, 1000, 2000, 40, &ErrorCode);
}
void CMaxonMotor::SetCurModeParameter(int) {
}

void CMaxonMotor::GetSupply(unsigned short &  piVoltage, short int& piCurrent) {

	piVoltage = 0;
	piCurrent = 0;
	unsigned short pAnalogValue=0, InputNumber=1;

	VCS_GetCurrentIs(keyHandle, nodeID, &piCurrent, &ErrorCode);
	VCS_GetAnalogInput(keyHandle, nodeID, InputNumber, &pAnalogValue, &ErrorCode);
	piVoltage = pAnalogValue;
}
