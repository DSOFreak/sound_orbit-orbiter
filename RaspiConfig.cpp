#include "RaspiConfig.h"


//id 0: ending 25.
//id 1: ending 26.
// id 2: ending 22
const int RaspiConfig::ownIndex =2; //0ok2ok





float RaspiConfig::fWheelPerimeterRaspi = 0;
RaspiConfig::RaspiConfig()
{
	if (ownIndex == 2)
	{
		fWheelPerimeterRaspi = 0.0303359 * 3.1415;
	}
	else if (ownIndex == 1)
	{
		fWheelPerimeterRaspi = 0.0303358 * 3.1415;
	}
	else
	{
		fWheelPerimeterRaspi = 0.0303359 * 3.1415;
	}
}


RaspiConfig::~RaspiConfig()
{
}
