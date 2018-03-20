#include "Toolbox.h"


/**
	This namespace has to be used for utiliy functions.
*/
namespace Toolbox {
	
	/**

		hostData format =
		1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10| 11| 12| 13| 14| 15| 16| 17| 18| 19| 20| 21| 22| 23| 24| 25| 26| 27| 28| 29| 30| 31| 32| 33| 34| 35| 36| 37| 38| 39| 40| 41
		A | A | B | C | C | C | . | C | C | D | D | D | . | D | D | I | I | J | J | J | J | J | J | K | K | L | E | E | F | G | G | G | . | G | G | H | H | H | . | H | H 
		
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

		if (hostData.length() < (speakerID + 1) * 26) {
			std::cout << "Error: Not enough data for the selected speaker id." << std::endl;
			d.direction = 0;
			d.speed = 0.0f;
			d.angularDistance = 0.0f;

			return d;
		}
		int startIdx = speakerID * 27;
		try {
			int realSpeakerID = std::stoi(hostData.substr(startIdx + 0, 2), nullptr, 10);
			int motorDirection = std::stoi(hostData.substr(startIdx + 2, 1), nullptr, 10);
			int speed = std::stoi(hostData.substr(startIdx + 3, 3), nullptr, 10);
			int speedDiv100 = std::stoi(hostData.substr(startIdx + 7, 2), nullptr, 10);

			int angularDistance = std::stoi(hostData.substr(startIdx + 9, 3), nullptr, 10);
			int angularDistanceDiv100 = std::stoi(hostData.substr(startIdx + 13, 2), nullptr, 10);

			int stim_nr = std::stoi(hostData.substr(startIdx + 15, 2), nullptr, 10);

			int stim_dur = std::stoi(hostData.substr(startIdx + 17, 6), nullptr, 10);
			float stim_vol_L_real = std::stoi(hostData.substr(startIdx + 23, 2), nullptr, 10);
			int stim_tbt = std::stoi(hostData.substr(startIdx + 25, 1), nullptr, 10);

			d.direction = motorDirection;
			d.speed = (1.0f * speed) + (0.01f * speedDiv100);
			d.angularDistance = (1.0f * angularDistance) + (0.01f * angularDistanceDiv100);
			d.stimulusDuration = stim_dur;
			d.stimulus_nr = stim_nr;
			float C = 0.0f;
			float Lmax = 90.0f;
			d.loudness = pow(10.0f, (stim_vol_L_real - Lmax + C) / 20.0f);
			d.toBeTriggerd = stim_tbt;
		}
		catch (std::exception e) {
			std::cout << e.what() << std::endl;
		}

		return d;
	}
}
