#include "tcpParameterRequestHandler.h"
#include <algorithm>
#include "StimuliLibrary.h"
#include "Movement.h"
#include <thread>
#include "DataToCSV.h"
const std::string tcpParameterRequestHandler::strEndIndicatorForProtocol = "Q"; // End of Telegram
tcpParameterRequestHandler::tcpParameterRequestHandler()
{
	llReferenceTimeToBlock = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	m_pMaxonMotor = CMaxonMotor::getInstance();
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
				std::cout << "S_N_CSV received: Start motor data recording " << endl;
				DataToCSV* pDataToCSV = DataToCSV::getInstance();
				// Stop the task (if started..just to make sure)
				DataToCSV::mutexDataToCSVTaskChecker.lock();
				DataToCSV::bContinueTask = false;
				DataToCSV::mutexDataToCSVTaskChecker.unlock();
				// Wait
				std::this_thread::sleep_for(std::chrono::milliseconds(pDataToCSV->uiUpdateRateMs));

				// Create the new file and write the information in strRequest to the file
				pDataToCSV->vCreateNewCSVFile(strRequest);

				// Start the task ;
				DataToCSV::mutexDataToCSVTaskChecker.lock();
				DataToCSV::bContinueTask = true;
				DataToCSV::mutexDataToCSVTaskChecker.unlock();
				//std::cout << "DataToCSV::bContinueTask " << DataToCSV::bContinueTask << endl;
				pDataToCSV->vTaskCyclicWriteOfMotorData();

				// set the testflag for MAA
				if (pDataToCSV->strTestName.find("MAA") != std::string::npos)
				{
					m_pMaxonMotor->vSetIsAnMAATestFlag(true);
				}
				else
				{
					m_pMaxonMotor->vSetIsAnMAATestFlag(false);
				}

			}
		}
		else if (strRequest.find("S_S_CSV") != std::string::npos) // Stop CSV recording
		{
			std::cout << "S_S_CSV received: Stop motor data recording " << endl;
			DataToCSV::mutexDataToCSVTaskChecker.lock();
			DataToCSV::bContinueTask = false;
			DataToCSV::mutexDataToCSVTaskChecker.unlock();
		}
	}
	else
	{
		// ERROR
		printf("ERROR: No reason to be in interpretRequest. Protocol interpretation is wrong \n");
		exit(-1);
	}
	printf("DEBUG:A \n");
	// convert to str
	std::string answerToServerRequest;
	if (!vecdResult.empty())
	{
		printf("DEBUG:B \n");
		strResultTemp << vecdResult.at(0);
		answerToServerRequest = strResultTemp.str();
		printf("DEBUG:C\n");
		//Add telegram delimiter
		answerToServerRequest.append(tcpParameterRequestHandler::strEndIndicatorForProtocol);
		//Add own speaker ID
		strResultTemp.str("");
		strResultTemp << RaspiConfig::ownIndex;
		printf("DEBUG:D \n");
		std::string strSpeakerIdentification = strRequest + "_ID_" + strResultTemp.str() + "_";
		answerToServerRequest.insert(0, strSpeakerIdentification);
		printf("DEBUG:E \n");
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
