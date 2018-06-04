#pragma once
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <exception>
#include <cmath>
// Protocol is 
// SpeakerIDx(00-99) Motordirection(0, 1, 2) Speed(000.00 - 999.00) AngularDistance(000.00 - 360.00) Queued/Instant Movement Trigger (0,1)  ... 
// StimulusNumber (00-99) StimulusDuration (000000-999999) dBSPL (00-99) StimulusToBetriggerd (0 - 1) Queued/Instant Stimulus Trigger (0,1))
namespace Toolbox
{
	struct HostData {
		u_int8_t direction;
		float speed;
		float angularDistance;
		u_int8_t stimulus_nr;
		u_int32_t stimulusDuration;
		float loudness;
		bool toBeTriggerd;
		bool mov_queued;
		bool stim_queued;
	};



	HostData decodeHostData(std::string hostData, size_t speakerID);

	bool bCheckForValidity(HostData &refHostData, std::string &strHostData, size_t speakerID, int iRealSpeakerID, int iStimulusNumber, int iStimulusDuration, int iAngularDistance);

	void vSetHostDataToZero(HostData &structHostData);
};

