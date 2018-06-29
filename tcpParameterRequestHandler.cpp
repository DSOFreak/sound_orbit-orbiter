#include "tcpParameterRequestHandler.h"



tcpParameterRequestHandler::tcpParameterRequestHandler(CMaxonMotor * pMaxonMotor) : m_pMaxonMotor(pMaxonMotor)
{
}


tcpParameterRequestHandler::~tcpParameterRequestHandler()
{
}

std::vector<double> tcpParameterRequestHandler::interpretRequest( std::string & strRequest)
{
	std::vector<double> vecdResult;;
	char charIsGetOrSetRequest = strRequest.at(0);
	if (charIsGetOrSetRequest == 'G')
	{
		if (strRequest == "G_V_A") // get all battery voltages
		{
			vecdResult.push_back(dGetBatteryVoltage());
		}
	}
	else if (charIsGetOrSetRequest == 'S')
	{
		// TO DO: Hier habe ich noch nicht das sich lohnen würde.. nichts zu setzen!
	}
	else
	{
		// ERROR
		printf("ERROR: No reason to be in interpretRequest. Protocol interpretation is wrong \n");
		exit(-1);
	}


	return vecdResult;
}

double tcpParameterRequestHandler::dGetBatteryVoltage()
{
	double dRetVal;
	unsigned short  piVoltage;
	short int piCurrent;
	// Requests the battery voltage (or the voltage applied to the raspi in general... could also be a constant voltage source)
	printf("dGetBatteryVoltage\n");
	m_pMaxonMotor->GetSupply(piVoltage, piCurrent);
	printf("dGetBatteryVoltage OK\n");
	dRetVal = double(piVoltage);
}
