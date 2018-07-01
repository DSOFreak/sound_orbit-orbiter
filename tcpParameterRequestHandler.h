#pragma once
#include "cmaxonmotor.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>
class tcpParameterRequestHandler
{
public:
	tcpParameterRequestHandler(std::shared_ptr<CMaxonMotor> pMaxonMotor);
	~tcpParameterRequestHandler();
	std::vector<double> interpretRequest( std::string & strRequest);
private:

	std::shared_ptr<CMaxonMotor>  m_pMaxonMotor;
	// Get Functions
	double dGetBatteryVoltage();
};

