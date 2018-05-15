#pragma once
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <exception>
#include <cmath>
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
};

