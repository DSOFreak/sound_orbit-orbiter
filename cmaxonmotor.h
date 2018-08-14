#ifndef CMAXONMOTOR_H
#define CMAXONMOTOR_H
#include <memory>
#include "Toolbox.h"
#include "Definitions.h" // Maxon Motor Header file
#include <ctime>
#define TRUE 1
#define FALSE 0

#define NO_MOVEMENT_IN_PROCESS -1
class CMaxonMotor
{
private:
	// DEBUG
	std::clock_t start;
	double duration;
	double dOverallDuration;

    char* PortName;
    unsigned short nodeID;
    void *keyHandle;//

	long lCurrentTargetPositionInMotorData;


	int iProfileVelocity_m; // prev. 10000
	int iProfileAcceleration_m; //prev. 5000
	int iProfileDeceleration_m; //prev. 10000
public:
	long currenTargetPos;
	long lgetCurrentTargetPositionInMotorData();
	long setCurrentTargetPositionInMotorData(long lTargetPositionInMotorData);
	long lConvertAngleInDegreeToMotorData(int iAngle);
    // ********* I N I T *********
	CMaxonMotor();
    CMaxonMotor(char[], unsigned short );
	bool reachedTarget(long long numberOfTimerCalls, long long numberOfMovementcalls, long long numberOfTCPCalls);

	unsigned int ErrorCode;
	double dStatus;
	std::shared_ptr<Toolbox::HostData> currentlyProcessedMovementData;

    void initializeDevice();
	
    void closeDevice();
    void EnableDevice();
    void DisableDevice();
    void Move(long TargetPosition);
	void getCurrentlyProcessedTargetPosition(long int &targetPosition);
    void getCurrentPosition(int& CurrentPosition);
	void vResetTargetPositionToCurrentPosition();
    void Halt();
    void activate_device();
	void SetPosModeParameter();
	void SetCurModeParameter(int);
	void setSpeed(float speed);
	void Error(unsigned int ErrorCode);
	void ErrorNbr(unsigned char * cErrorCode);

	void GetSupply(unsigned short& piVoltage, short int& piCurrent);

	bool bTryToAddMovementDataToCurrentMovement(); // if the direction and speed and speaker is ok -> increase current movement distance
	//DEBUG
	int GetCPULoad(float &load);
};

#endif // CMAXONMOTOR_H

