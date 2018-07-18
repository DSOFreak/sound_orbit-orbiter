#ifndef CMAXONMOTOR_H
#define CMAXONMOTOR_H

#include "Definitions.h" // Maxon Motor Header file

#define TRUE 1
#define FALSE 0


class CMaxonMotor
{
private:
    char* PortName;
    unsigned short nodeID;
    void *keyHandle;//

	long lCurrentTargetPositionInMotorData;
public:
	long lgetCurrentTargetPositionInMotorData();
	long setCurrentTargetPositionInMotorData(long lTargetPositionInMotorData);
	long lConvertAngleInDegreeToMotorData(int iAngle);
    // ********* I N I T *********
	CMaxonMotor();
    CMaxonMotor(char[], unsigned short );
	bool reachedTarget();

	unsigned int ErrorCode;
	double dStatus;


    void initializeDevice();
	
    void closeDevice();
    void EnableDevice();
    void DisableDevice();
    void Move(long TargetPosition);
	void getTargetPosition(long int &targetPosition);
    void getCurrentPosition(int& CurrentPosition);
    void Halt();
    void activate_device();
	void SetPosModeParameter();
	void SetCurModeParameter(int);
	void setSpeed(float speed);
	void Error(unsigned int ErrorCode);
	void ErrorNbr(unsigned char * cErrorCode);

	void GetSupply(unsigned short& piVoltage, short int& piCurrent);

	//DEBUG
	int GetCPULoad(float &load);
};

#endif // CMAXONMOTOR_H

