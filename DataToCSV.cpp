#include "DataToCSV.h"
#include <thread>

DataToCSV* DataToCSV::pInstance = nullptr;
bool DataToCSV::bContinueTask = false;
DataToCSV::DataToCSV( ) 
{
	strDelimeter = ",";
	ulLinesCount = 0;
}


DataToCSV::~DataToCSV()
{
}
DataToCSV*  DataToCSV::getInstance()
{
	if (!pInstance)
	{
		pInstance = new DataToCSV();
	}
	return pInstance;
}

void DataToCSV::vTaskCyclicWriteOfMotorData(std::shared_ptr<CMaxonMotor> pMaxonMotor, bool bContinueTask_)
{
	std::vector<std::string> vecstrInputData;
	// RaspiTimestamp
	long long  llRaspiTimestamp;
	std::string strRaspiTimestamp;
	// Motor Data
	int iCurrentPosition;
	std::thread vTaskCyclicWriteOfMotorDataThread{ [&]()
	{
		while (bContinueTask_)
		{
			printf("INSIDE TASK");
			vecstrInputData.clear();
			// Motordata
			pMaxonMotor->getCurrentPosition(iCurrentPosition);
			vecstrInputData.push_back(std::to_string(iCurrentPosition));

			// Timestamp
			llRaspiTimestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			std::string strRaspiTimestamp = std::to_string(llRaspiTimestamp);
			vecstrInputData.push_back(strRaspiTimestamp);

			addDatainRow(vecstrInputData.begin(), vecstrInputData.end());

			mutexDataToCSVTaskChecker.lock();
			bContinueTask_ = DataToCSV::bContinueTask;
			mutexDataToCSVTaskChecker.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(uiUpdateRateMs));
		}
		vCloseFile();
	} }; vTaskCyclicWriteOfMotorDataThread.detach();	
}
