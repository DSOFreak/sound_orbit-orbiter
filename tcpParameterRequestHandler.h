#pragma once
#include "cmaxonmotor.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include "RaspiConfig.h"
#include "Toolbox.h"
class tcpParameterRequestHandler
{
public:
	tcpParameterRequestHandler();
	~tcpParameterRequestHandler();
	std::string interpretRequest( std::string & strRequest);
private:
	static const std::string strEndIndicatorForProtocol;
	CMaxonMotor*  m_pMaxonMotor;
	// Get Functions
	double dGetBatteryVoltage();
	double dGetMotorPosition();
	const std::string strFirstLineOfNewCSVFile = "MotorPosition, RaspiTimeStamp, Name, Surname, TestName, TestSetting, PCTimeStampOfCreation \n";
	long long llReferenceTimeToBlock;//das ist aus der eile entstanden
};

