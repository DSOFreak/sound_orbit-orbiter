#include "tcpParameterRequestHandler.h"
#include <algorithm>
#include "StimuliLibrary.h"
#include "Movement.h"
#include <thread>
#include "DataToCSV.h"
const std::string tcpParameterRequestHandler::strEndIndicatorForProtocol = "Q"; // End of Telegram
tcpParameterRequestHandler::tcpParameterRequestHandler(std::shared_ptr<CMaxonMotor> pMaxonMotor) : m_pMaxonMotor(pMaxonMotor)
{
	llReferenceTimeToBlock = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
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
		if (strRequest == "S_C_M") // clear all movements
		{
			std::shared_ptr<Movement> pMovement = Movement::getInstance();
			pMovement->vClearMovementQueue();
			m_pMaxonMotor->vResetTargetPositionToCurrentPosition();
			printf("Movement Queue Cleared \n");
			// Debug clear also stimuliLib
			std::shared_ptr<StimuliLibrary> pStimuliLib = StimuliLibrary::getInstance();
			pStimuliLib->vSetResetStimuliLib(true);

			//pStimuliLib->vSetUp();
			printf("vSetResetStimuliLib(true)\n");
			// -> Vielleicht auch auf nullptr setzen und die Bewegung sofort stoppen (Motor->Stop) schicken
			// -> Dann das Kommando umbenennen in S_C_A Set Clear All
		}
		else if (strRequest.find("S_N_CSV") != std::string::npos) // Set new CSV
		{
			long long llCurrentTimestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			if (llCurrentTimestamp - llReferenceTimeToBlock > 4000) // to avoid false creation of databases if the tcp protocol is messy
			{
				llReferenceTimeToBlock = llCurrentTimestamp;
				llReferenceTimeToBlock = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
				std::cout << "S_N_CSV received " << endl;
				DataToCSV* pDataToCSV = DataToCSV::getInstance();
				// Stop the task (if started)
				DataToCSV::bContinueTask = false;
				// Wait
				std::this_thread::sleep_for(std::chrono::milliseconds(pDataToCSV->uiUpdateRateMs));

				// Create the new file and write the information in strRequest to the file
				pDataToCSV->vCreateNewCSVFile(strRequest);

				// Start the task ;
				DataToCSV::bContinueTask = true;
				std::cout << "DataToCSV::bContinueTask " << DataToCSV::bContinueTask << endl;
				pDataToCSV->mutexDataToCSVTaskChecker.unlock();
				std::cout << "pDataToCSV->vTaskCyclicWriteOfMotorData(m_pMaxonMotor);	" << endl;
				pDataToCSV->vTaskCyclicWriteOfMotorData(m_pMaxonMotor, DataToCSV::bContinueTask);
			}

		}
	}
	else
	{
		// ERROR
		printf("ERROR: No reason to be in interpretRequest. Protocol interpretation is wrong \n");
		exit(-1);
	}

	// convert to str
	std::string answerToServerRequest;
	if (!vecdResult.empty())
	{
		strResultTemp << vecdResult.at(0);
		answerToServerRequest = strResultTemp.str();

		//Add telegram delimiter
		answerToServerRequest.append(tcpParameterRequestHandler::strEndIndicatorForProtocol);
		//Add own speaker ID
		strResultTemp.str("");
		strResultTemp << RaspiConfig::ownIndex;
		std::string strSpeakerIdentification = strRequest + "_ID_" + strResultTemp.str() + "_";
		answerToServerRequest.insert(0, strSpeakerIdentification);
	}
	else
	{
		answerToServerRequest = "NO_ANSWER_OF_SET_REQUEST";
	}

	return answerToServerRequest;
}

double tcpParameterRequestHandler::dGetBatteryVoltage()
{
	double dRetVal;
	unsigned short  piVoltage;
	short int piCurrent;
	// Requests the battery voltage (or the voltage applied to the raspi in general... could also be a constant voltage source)
	m_pMaxonMotor->GetSupply(piVoltage, piCurrent);
	// �bernommen aus displayfunc() (unklar was f�r ein faktor das ist)
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
