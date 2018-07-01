#include "tcpParameterRequestHandler.h"


const std::string tcpParameterRequestHandler::strEndIndicatorForProtocol = "Q"; // End of Telegram
tcpParameterRequestHandler::tcpParameterRequestHandler(std::shared_ptr<CMaxonMotor> pMaxonMotor) : m_pMaxonMotor(pMaxonMotor)
{
}


tcpParameterRequestHandler::~tcpParameterRequestHandler()
{
}

std::string tcpParameterRequestHandler::interpretRequest( std::string & strRequest)
{
	// ANSWER PROTOCL IS "REQUEST_ID_idnr_Value" e.g. G_V_A_ID_0_12.392
	std::ostringstream strResultTemp;
	std::vector<double> vecdResult;
	char charIsGetOrSetRequest = strRequest.at(0);
	if (charIsGetOrSetRequest == 'G')
	{
		if (strRequest == "G_V_A") // get all battery voltages
		{
			vecdResult.push_back(dGetBatteryVoltage());
		}
		else if("G_P_A") // get all motor positions (raw DEBUG)
		{
			vecdResult.push_back(dGetMotorPosition());
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

	// convert to str
	strResultTemp << vecdResult.at(0);



	std::string answerToServerRequest = strResultTemp.str();

	//Add telegram delimiter
	answerToServerRequest.append(tcpParameterRequestHandler::strEndIndicatorForProtocol);
	//Add own speaker ID
	strResultTemp.str("");
	strResultTemp << RaspiConfig::ownIndex;
	std::string strSpeakerIdentification = strRequest +"_ID_" + strResultTemp.str() + "_";
	answerToServerRequest.insert(0, strSpeakerIdentification);
	return answerToServerRequest;
}

double tcpParameterRequestHandler::dGetBatteryVoltage()
{
	double dRetVal;
	unsigned short  piVoltage;
	short int piCurrent;
	// Requests the battery voltage (or the voltage applied to the raspi in general... could also be a constant voltage source)
	m_pMaxonMotor->GetSupply(piVoltage, piCurrent);
	// übernommen aus displayfunc() (unklar was für ein faktor das ist)
	dRetVal = (double)(4.25 * double(piVoltage) / 1000);
	return dRetVal;
}

double tcpParameterRequestHandler::dGetMotorPosition()
{
	double dRetVal;
	int iCurrentPosition;
	// Requests the current motor position
	m_pMaxonMotor->getCurrentPosition(iCurrentPosition);
	dRetVal = (double)iCurrentPosition;
	return dRetVal;
}
