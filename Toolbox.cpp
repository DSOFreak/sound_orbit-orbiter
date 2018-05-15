#include "Toolbox.h"


/**
	This namespace has to be used for utiliy functions.
*/
namespace Toolbox {
	
	/**
						  1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10| 11| 12| 13| 14| 15| 16| 17| 18| 19| 20| 21| 22| 23| 24| 25| 26| 27| 28| 29| 30
		hostData format = A | A | B | C | C | C | . | C | C | D | D | D | . | D | D | E | E | F | G | G | G | . | G | G | H | H | H | . | H | H 
		A = speaker id  = 00
		B = moving direction
		C = Speed with two decimals
		D = angle with two decimals
		E = speaker id = 01
		F = moving direction
		G = Speed with two decimals
		H = angle with two decimals
	*/
	HostData decodeHostData(std::string hostData, size_t speakerID)
	{
		HostData d;

		if (hostData.length() < (speakerID + 1) * 15) {
			std::cout << "Error: Not enough data for the selected speaker id." << std::endl;
			d.direction = 0;
			d.speed = 0.0f;
			d.angularDistance = 0.0f;

			return d;
		}
		int startIdx = speakerID * 16;
		int realSpeakerID = std::stoi(hostData.substr(startIdx + 0, 2), nullptr, 10);
		int motorDirection = std::stoi(hostData.substr(startIdx + 2, 1), nullptr, 10);
		int speed = std::stoi(hostData.substr(startIdx + 3, 3), nullptr, 10);
		int speedDiv100 = std::stoi(hostData.substr(startIdx + 7, 2), nullptr, 10);

		int angularDistance = std::stoi(hostData.substr(startIdx + 9, 3), nullptr, 10);
		int angularDistanceDiv100 = std::stoi(hostData.substr(startIdx + 13, 2), nullptr, 10);

		d.direction = motorDirection;
		d.speed = (1.0f * speed) + (0.01f * speedDiv100);
		d.angularDistance = (1.0f * angularDistance) + (0.01f * angularDistanceDiv100);

		return d;
	}
}
