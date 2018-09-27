#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <iterator>
#include <string>
#include <algorithm>
#include <string>
#include <chrono>
#include <mutex>
#include "cmaxonmotor.h"
#include "RaspiConfig.h"
using namespace std::chrono;
using namespace std;
class DataToCSV
{
	std::fstream fstrFile;
	std::string strFileName;
	std::string strDelimeter;
	unsigned long ulLinesCount;

public:
	std::string strTestName;
	DataToCSV();
	~DataToCSV();
	static bool bContinueTask;
	const unsigned int uiUpdateRateMs = 180;
	static DataToCSV* pInstance;
	static DataToCSV * getInstance();
	void vTaskCyclicWriteOfMotorData();

	static std::mutex mutexDataToCSVTaskChecker;
	/*
	* Member function to store a range as comma seperated value
	*/
	template<typename T>

	/*
	This Function accepts a range and appends all the elements in the range
	to the last row, seperated by delimeter(Default is comma)

	https://thispointer.com/how-to-write-data-in-a-csv-file-in-c/
	*/
	inline void addDatainRow(T first, T last)
	{
		// Iterate over the range and add each lement to file seperated by delimeter.
		for (; first != last; )
		{
			fstrFile << *first;
			if (++first != last)
				fstrFile << strDelimeter;
		}
		fstrFile << "\n";
		ulLinesCount++;
	}

	inline void vCloseFile()
	{

		fstrFile.close();
	}

	inline void vOpenFile()
	{
		// Open the file in truncate mode if first line else in Append Mode
		fstrFile.open(strFileName, std::ios::out | (ulLinesCount ? std::ios::app : std::ios::trunc));
	}

	inline void vCreateNewCSVFile(std::string strHeaderData)
	{
		vCloseFile();
		std::size_t stIdx = strHeaderData.find("CSV");
		//Name
		int iNameIdxBegin = stIdx + 4;
		stIdx = strHeaderData.find("_", iNameIdxBegin);
		int iNameIdxEnd = stIdx;
		cout << "iNameIdxBegin " << iNameIdxBegin << endl;
		cout << "iNameIdxEnd " << iNameIdxEnd << endl;
		std::string strName = strHeaderData.substr(iNameIdxBegin, iNameIdxEnd- iNameIdxBegin);
		cout <<"strName "<< strName << endl;

		//Surname
		stIdx = strHeaderData.find("_", stIdx);
		int iSurnameIdxBegin = stIdx + 1;
		stIdx = strHeaderData.find("_", stIdx + 1);
		int iSurnameIdxEnd = stIdx;
		std::string strSurname = strHeaderData.substr(iSurnameIdxBegin, iSurnameIdxEnd- iSurnameIdxBegin);
		cout << "strSurname "<< strSurname << endl;

		//TestName
		stIdx = strHeaderData.find("_", stIdx);
		int iTestNameIdxBegin = stIdx + 1;
		stIdx = strHeaderData.find("_", stIdx + 1);
		int iTestNameIdxEnd = stIdx;
		strTestName = strHeaderData.substr(iTestNameIdxBegin, iTestNameIdxEnd- iTestNameIdxBegin);
		cout << "strTestName " << strTestName << endl;

		//TestSetting
		stIdx = strHeaderData.find("_", stIdx);
		int iTestSettingIdxBegin = stIdx + 1;
		stIdx = strHeaderData.find("_", stIdx + 1);
		int iTestSettingIdxEnd = stIdx;
		std::string strTestSetting = strHeaderData.substr(iTestSettingIdxBegin, iTestSettingIdxEnd- iTestSettingIdxBegin);
		cout << "strTestSetting " << strTestSetting << endl;

		//PCTimeStamp
		stIdx = strHeaderData.find("_", stIdx);
		int iPCTimeStampIdxBegin = stIdx + 1;
		stIdx = strHeaderData.find("_", stIdx + 1);
		int iPCTimeStampIdxEnd = stIdx;
		std::string strPCTimestamp = strHeaderData.substr(iPCTimeStampIdxBegin, iPCTimeStampIdxEnd- iPCTimeStampIdxBegin);
		cout << "strPCTimestamp " << strPCTimestamp << endl;

		// Start of the speaker by GUI
		stIdx = strHeaderData.find("_", stIdx);
		float fPositionSpkr0InDegreeByGuiIdxBegin = stIdx + 1;
		stIdx = strHeaderData.find("_", stIdx + 1);
		float fPositionSpkr0InDegreeByGuiIdxEnd = stIdx;
		std::string strPositionSpkr0InDegreeByGui = strHeaderData.substr(fPositionSpkr0InDegreeByGuiIdxBegin, fPositionSpkr0InDegreeByGuiIdxEnd - fPositionSpkr0InDegreeByGuiIdxBegin);
		cout << "strPositionSpkr0InDegreeByGuiIdxBegin " << strPositionSpkr0InDegreeByGui << endl;

		stIdx = strHeaderData.find("_", stIdx);
		float fPositionSpkr1InDegreeByGuiIdxBegin = stIdx + 1;
		stIdx = strHeaderData.find("_", stIdx + 1);
		float fPositionSpkr1InDegreeByGuiIdxEnd = stIdx;
		std::string strPositionSpkr1InDegreeByGui = strHeaderData.substr(fPositionSpkr1InDegreeByGuiIdxBegin, fPositionSpkr1InDegreeByGuiIdxEnd - fPositionSpkr1InDegreeByGuiIdxBegin);
		cout << "strPositionSpkr1InDegreeByGui " << strPositionSpkr1InDegreeByGui << endl;

		stIdx = strHeaderData.find("_", stIdx);
		float fPositionSpkr2InDegreeByGuiIdxBegin = stIdx + 1;
		stIdx = strHeaderData.find("_", stIdx + 1);
		float fPositionSpkr2InDegreeByGuiIdxEnd = stIdx;
		std::string strPositionSpkr2InDegreeByGui = strHeaderData.substr(fPositionSpkr2InDegreeByGuiIdxBegin, fPositionSpkr2InDegreeByGuiIdxEnd - fPositionSpkr2InDegreeByGuiIdxBegin);
		cout << "strPositionSpkr2InDegreeByGui " << strPositionSpkr2InDegreeByGui << endl;

		// RaspiTimestamp
		long long  llRaspiTimestamp = duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count();
		std::string strRaspiTimestamp = std::to_string(llRaspiTimestamp);
		cout << "strRaspiTimestamp " << strRaspiTimestamp << endl;


		strFileName = "../../../../" + strTestName + "_" + strTestSetting  +"_" + strName +"_" + +"_"+ strPCTimestamp + "_"+ std::to_string(RaspiConfig::ownIndex) + ".csv";
		cout << strFileName << endl;
		std::vector<std::string> vecstrHeadlines = { "MotorPosition", "RaspiTimeStamp", "Name", "Surname", "TestName","TestSetting", "PCTimeStampOfCreation", "PosInDegreeSpkr0GivenByGUIData", "PosInDegreeSpkr1GivenByGUIData" , "PosInDegreeSpkr2GivenByGUIData" };
		std::vector<std::string> vecstrFirstLine= { "-", strRaspiTimestamp, strName, strSurname, strTestName, strTestSetting, strPCTimestamp,strPositionSpkr0InDegreeByGui,strPositionSpkr1InDegreeByGui,strPositionSpkr2InDegreeByGui };
		vOpenFile();

		addDatainRow(vecstrHeadlines.begin(), vecstrHeadlines.end());
		addDatainRow(vecstrFirstLine.begin(), vecstrFirstLine.end());
	}
};

