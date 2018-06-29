#pragma once
#include "cmaxonmotor.h"
#include <vector>
#include <string>
#include <iostream>
class tcpParameterRequestHandler
{
public:
	tcpParameterRequestHandler(CMaxonMotor *pMaxonMotor);
	~tcpParameterRequestHandler();
	std::vector<double> interpretRequest( std::string & strRequest);
private:

	CMaxonMotor * m_pMaxonMotor;
	// Get Functions
	double dGetBatteryVoltage();
};

