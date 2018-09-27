#include "Toolbox.h"
#include <cstdio>		
#include <stdio.h>
#include <string.h>
#include "RaspiConfig.h"
#define MAX_STIMULUS_NUMBER 3
#define MAX_SPEAKER_NUMBER 2
#define SMALLEST_ACCEPTED_ANGLE_DEGREE 0.25
#define VALUE_NOT_SET -1
using namespace std;
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
		I = 
	
		E = speaker id = 01
		F = moving directions
		G = Speed with two decimals
		H = angle with two decimals

	*/
	HostData decodeHostData(std::string hostData)
	{
		HostData d;

		int transmittedSpeakerID = VALUE_NOT_SET;
		for (int i = 0; i <= MAX_SPEAKER_NUMBER; i++)
		{
			if (transmittedSpeakerID == RaspiConfig::ownIndex)
			{
				//printf("\n \n \n \n \n \n Found the speaker \n \n \n");
				break;
			}
			int startIdx = i * uiLengthOfProtocol;

			try {
				transmittedSpeakerID = std::stoi(hostData.substr(startIdx + 0, 2), nullptr, 10);
				int motorDirection = std::stoi(hostData.substr(startIdx + 2, 1), nullptr, 10);
				int speed = std::stoi(hostData.substr(startIdx + 3, 3), nullptr, 10);
				int speedDiv100 = std::stoi(hostData.substr(startIdx + 7, 2), nullptr, 10);

				int angularDistance = std::stoi(hostData.substr(startIdx + 9, 3), nullptr, 10);
				int angularDistanceDiv100 = std::stoi(hostData.substr(startIdx + 13, 2), nullptr, 10);
				float fAngularDist = (1.0f * (float)angularDistance) + (0.01f * (float)angularDistanceDiv100);;
				int queued_mov_nr = std::stoi(hostData.substr(startIdx + 15, 1), nullptr, 10);

				int stim_nr = std::stoi(hostData.substr(startIdx + 16, 2), nullptr, 10);

				int stim_dur = std::stoi(hostData.substr(startIdx + 18, 6), nullptr, 10);
				float stim_vol_L_real = std::stoi(hostData.substr(startIdx + 24, 2), nullptr, 10);
				int stim_tbt = std::stoi(hostData.substr(startIdx + 26, 1), nullptr, 10);
				int queued_stim_nr = std::stoi(hostData.substr(startIdx + 27, 1), nullptr, 10);


				if (bCheckForValidity(d, stim_tbt, hostData, transmittedSpeakerID, stim_nr, stim_dur, fAngularDist))
				{
					//printf("This protocol is valid \n");
					d.speakerIDX = transmittedSpeakerID;
					d.direction = motorDirection;
					d.speed = (1.0f * speed) + (0.01f * speedDiv100);
					d.fAngularDistance = (1.0f * (float)angularDistance) + (0.01f * (float)angularDistanceDiv100);
					d.mov_queued = queued_mov_nr == 0;
					d.stimulusDuration = stim_dur;
					d.stimulus_nr = stim_nr;
					float C = 13.3f; // CALIBRATION FACTOR
					float Lmax = 90.0f;
					d.loudness = pow(10.0f, (stim_vol_L_real - Lmax + C) / 20.0f);
					//printf("d.loudness %f \n", d.loudness);
					d.toBeTriggerd = stim_tbt;
					d.stim_queued = queued_stim_nr == 0;


					std::cout << "hostData: " << "dir = " << static_cast<int>(d.direction) << ", angleToMove = " <<d.fAngularDistance << ", speed = " << d.speed << std::endl;
					std::cout << "stim_nr = " << static_cast<int>(d.stimulus_nr) << ", stim_dur = " << d.stimulusDuration << ", vol = " << d.loudness << ", stimulusToBeTriggerd = " << d.toBeTriggerd << std::endl;


				}
			}
			catch (std::exception e) {
				vSetHostDataToZero(d);
				printf("Catch: This protocol is nothing to process for the motor / audio control. \n");
		}
	}
		return d;
	}
	bool bCheckForValidity(HostData &refHostData, int iStimulusToBeTriggered, std::string &strHostData, unsigned int transmittedSpeakerID, int iStimulusNumber, int iStimulusDuration, float fAngularDistance)
	{

		if (transmittedSpeakerID != RaspiConfig::ownIndex)
		{
			//printf("This is a protocol not for my speaker ID. I am %d and this is %d \n", RaspiConfig::ownIndex, transmittedSpeakerID);
			vSetHostDataToZero(refHostData);
			return false;
		}
		else
		{
			// DUMMY: Dieser check ist nicht wasserdicht. wenn das protokoll steht finalisieren.
			if ((iStimulusToBeTriggered == 0) && (fAngularDistance < SMALLEST_ACCEPTED_ANGLE_DEGREE))
			{
				printf("Nothing to do here: No Movement and No Stimulus \n");
				vSetHostDataToZero(refHostData);
				return false;
			}
			else if ((MAX_SPEAKER_NUMBER< transmittedSpeakerID) || (transmittedSpeakerID < 0) || (MAX_STIMULUS_NUMBER < iStimulusNumber) || (iStimulusNumber <= 0))
			{
				printf("Check for validity: This protocol is nothing to process for the motor / audio control... \n");
				vSetHostDataToZero(refHostData);
				return false;
			}
			else
			{
				//printf("I am speaker number %d, I take the protocol with the transmittedSpeakerID %d \n", RaspiConfig::ownIndex, transmittedSpeakerID);
				return true;
			}
		}

	}
	void vSetHostDataToZero(HostData &structHostData)
	{
		structHostData.direction = 0;
		structHostData.speed = 0;
		structHostData.fAngularDistance = 0.0;
		structHostData.stimulus_nr = 0;
		structHostData.stimulusDuration = 0;
		structHostData.loudness = 0;
		structHostData.toBeTriggerd = 0;
		structHostData.mov_queued = 0;
		structHostData.stim_queued = 0;

	}
}
