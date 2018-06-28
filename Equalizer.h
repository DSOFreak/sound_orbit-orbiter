#ifndef EQUALIZER_H
#define EQUALIZER_H
#include <fmod.hpp>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include "StimuliLibrary.h"
#define NUMBER_OF_TERCEBANDS 20
#define NUMBER_OF_STIMULI 3
using namespace std;
class Equalizer
{
public:
	Equalizer(unsigned int uiSpeakerID, unsigned int uiStimulusNumber);
	~Equalizer();
	void initDSPWithEQSettings(FMOD::Channel* pChannel, FMOD::ChannelGroup* pChannelGroup, FMOD::System* pSystem);
private:
	//debug
	FMOD::DSP *pDSPChanHead;
	FMOD::DSP *pDSPChanGrpHead;



	unsigned int m_uiSpeakerID;
	unsigned int m_uiStimulusNumber;


	/* EQUALIZER DSP*/
	FMOD::DSP	*dsp_Eq_125Hz;
	FMOD::DSP	*dsp_Eq_160Hz;
	FMOD::DSP	*dsp_Eq_250Hz;
	FMOD::DSP	*dsp_Eq_315Hz;
	FMOD::DSP	*dsp_Eq_400Hz;
	FMOD::DSP	*dsp_Eq_500Hz;
	FMOD::DSP	*dsp_Eq_630Hz;
	FMOD::DSP	*dsp_Eq_800Hz;
	FMOD::DSP	*dsp_Eq_1000Hz;
	FMOD::DSP	*dsp_Eq_1250Hz;
	FMOD::DSP	*dsp_Eq_1600Hz;
	FMOD::DSP	*dsp_Eq_2500Hz;
	FMOD::DSP	*dsp_Eq_3150Hz;
	FMOD::DSP	*dsp_Eq_4000Hz;
	FMOD::DSP	*dsp_Eq_5000Hz;
	FMOD::DSP	*dsp_Eq_6300Hz;
	FMOD::DSP	*dsp_Eq_8000Hz;
	FMOD::DSP	*dsp_Eq_10000Hz;
	FMOD::DSP	*dsp_Eq_12500Hz;
	FMOD::DSP	*dsp_Eq_16000Hz;

	FMOD::DSP* arrEqDSPObjects[NUMBER_OF_TERCEBANDS] = {
		dsp_Eq_125Hz,
		dsp_Eq_160Hz,
		dsp_Eq_250Hz,
		dsp_Eq_315Hz,
		dsp_Eq_400Hz,
		dsp_Eq_500Hz,
		dsp_Eq_630Hz,
		dsp_Eq_800Hz,
		dsp_Eq_1000Hz,
		dsp_Eq_1250Hz,
		dsp_Eq_1600Hz,
		dsp_Eq_2500Hz,
		dsp_Eq_3150Hz,
		dsp_Eq_4000Hz,
		dsp_Eq_5000Hz,
		dsp_Eq_6300Hz,
		dsp_Eq_8000Hz,
		dsp_Eq_10000Hz,
		dsp_Eq_12500Hz,
		dsp_Eq_16000Hz
	};
	
	// Map mit dem paar (speaker number, 
	float myEqSettings[NUMBER_OF_TERCEBANDS][NUMBER_OF_STIMULI];

	/* WHIE NOISE (WN)*/
	const float arrfSpeaker0WN[NUMBER_OF_TERCEBANDS][NUMBER_OF_STIMULI] = { // CENTERFREQ, BANDWIDTH, GAIN  When a frequency has its gain set to 1.0, the sound will be unaffected and represents the original signal exactly.
		{ 125.0f,	0.2f,	1.0f },
		{ 160.0f,	0.2f,	1.0f },
		{ 250.0f,	0.2f,	1.0f },
		{ 315.0f,	0.2f,	1.0f },
		{ 400.0f,	0.2f,	1.0f },
		{ 500.0f,	0.2f,	1.0f },
		{ 630.0f,	0.2f,	1.0f },
		{ 800.0f,	0.2f,	1.0f },
		{ 1000.0f,	0.2f,	1.0f },
		{ 1250.0f,	0.2f,	1.0f },
		{ 1600.0f,	0.2f,	1.0f },
		{ 2500.0f,	0.2f,	1.0f },
		{ 3150.0f,	0.2f,	1.0f },
		{ 4000.0f,	0.2f,	1.0f },
		{ 5000.0f,	0.2f,	1.0f },
		{ 6300.0f,	0.2f,	1.0f },
		{ 8000.0f,	0.2f,	1.0f },
		{ 10000.0f,	0.2f,	1.0f },
		{ 12500.0f,	0.2f,	1.0f },
		{ 16000.0f,	0.2f,	1.0f },
	};

	const float arrfSpeaker1WN[NUMBER_OF_TERCEBANDS][NUMBER_OF_STIMULI] = { // CENTERFREQ, BANDWIDTH, GAIN  When a frequency has its gain set to 1.0, the sound will be unaffected and represents the original signal exactly.
		{ 125.0f,	0.2f,	1.0f },
		{ 160.0f,	0.2f,	1.0f },
		{ 250.0f,	0.2f,	1.0f },
		{ 315.0f,	0.2f,	1.0f },
		{ 400.0f,	0.2f,	1.0f },
		{ 500.0f,	0.2f,	1.0f },
		{ 630.0f,	0.2f,	1.0f },
		{ 800.0f,	0.2f,	1.0f },
		{ 1000.0f,	0.2f,	1.0f },
		{ 1250.0f,	0.2f,	1.0f },
		{ 1600.0f,	0.2f,	1.0f },
		{ 2500.0f,	0.2f,	1.0f },
		{ 3150.0f,	0.2f,	1.0f },
		{ 4000.0f,	0.2f,	1.0f },
		{ 5000.0f,	0.2f,	1.0f },
		{ 6300.0f,	0.2f,	1.0f },
		{ 8000.0f,	0.2f,	1.0f },
		{ 10000.0f,	0.2f,	1.0f },
		{ 12500.0f,	0.2f,	1.0f },
		{ 16000.0f,	0.2f,	1.0f },
	};

	const float arrfSpeaker2WN[NUMBER_OF_TERCEBANDS][NUMBER_OF_STIMULI] = { // CENTERFREQ, BANDWIDTH, GAIN  When a frequency has its gain set to 1.0, the sound will be unaffected and represents the original signal exactly.
		{ 125.0f,	0.2f,	1.0f },
		{ 160.0f,	0.2f,	1.0f },
		{ 250.0f,	0.2f,	1.0f },
		{ 315.0f,	0.2f,	1.0f },
		{ 400.0f,	0.2f,	1.0f },
		{ 500.0f,	0.2f,	1.0f },
		{ 630.0f,	0.2f,	1.0f },
		{ 800.0f,	0.2f,	1.0f },
		{ 1000.0f,	0.2f,	1.0f },
		{ 1250.0f,	0.2f,	1.0f },
		{ 1600.0f,	0.2f,	1.0f },
		{ 2500.0f,	0.2f,	1.0f },
		{ 3150.0f,	0.2f,	1.0f },
		{ 4000.0f,	0.2f,	1.0f },
		{ 5000.0f,	0.2f,	1.0f },
		{ 6300.0f,	0.2f,	1.0f },
		{ 8000.0f,	0.2f,	1.0f },
		{ 10000.0f,	0.2f,	1.0f },
		{ 12500.0f,	0.2f,	1.0f },
		{ 16000.0f,	0.2f,	1.0f },
	};
	/* PINK NOISE (WN)*/
	const float arrfSpeaker0PN[NUMBER_OF_TERCEBANDS][NUMBER_OF_STIMULI] = { // CENTERFREQ, BANDWIDTH, GAIN  When a frequency has its gain set to 1.0, the sound will be unaffected and represents the original signal exactly.

		{ 125.0f,	0.2f,	1.0f},
		{ 160.0f,	0.2f,	-5.0f },
		{ 250.0f,	0.2f,	1.0f},
		{ 315.0f,	0.2f,	1.0f},
		{ 400.0f,	0.2f,	1.0f},
		{ 500.0f,	0.2f,	1.0f},
		{ 630.0f,	0.2f,	1.0f},
		{ 800.0f,	0.2f,	-5.0f},
		{ 1000.0f,	0.2f,	1.0f},
		{ 1250.0f,	0.2f,	1.0f},
		{ 1600.0f,	0.2f,	1.0f},
		{ 2500.0f,	0.2f,	1.0f},
		{ 3150.0f,	0.2f,	-5.0f},//
		{ 4000.0f,	0.2f,	1.0f},
		{ 5000.0f,	0.2f,	1.0f},
		{ 6300.0f,	0.2f,	1.0f},
		{ 8000.0f,	0.2f,	1.0f},
		{ 10000.0f,	0.2f,	-5.0f},
		{ 12500.0f,	0.2f,	1.0f},//
		{ 16000.0f,	0.2f,	1.0f},
	};

	const float arrfSpeaker1PN[NUMBER_OF_TERCEBANDS][NUMBER_OF_STIMULI] = { // CENTERFREQ, BANDWIDTH, GAIN  When a frequency has its gain set to 1.0, the sound will be unaffected and represents the original signal exactly.
		{ 125.0f,	0.2f,	1.0f },
		{ 160.0f,	0.2f,	1.0f },
		{ 250.0f,	0.2f,	1.0f },
		{ 315.0f,	0.2f,	1.0f },
		{ 400.0f,	0.2f,	1.0f },
		{ 500.0f,	0.2f,	1.0f },
		{ 630.0f,	0.2f,	1.0f },
		{ 800.0f,	0.2f,	1.0f },
		{ 1000.0f,	0.2f,	1.0f },
		{ 1250.0f,	0.2f,	1.0f },
		{ 1600.0f,	0.2f,	1.0f },
		{ 2500.0f,	0.2f,	1.0f },
		{ 3150.0f,	0.2f,	1.0f },
		{ 4000.0f,	0.2f,	1.0f },
		{ 5000.0f,	0.2f,	1.0f },
		{ 6300.0f,	0.2f,	1.0f },
		{ 8000.0f,	0.2f,	1.0f },
		{ 10000.0f,	0.2f,	1.0f },
		{ 12500.0f,	0.2f,	1.0f },
		{ 16000.0f,	0.2f,	1.0f },
	};

	const float arrfSpeaker2PN[NUMBER_OF_TERCEBANDS][NUMBER_OF_STIMULI] = { // CENTERFREQ, BANDWIDTH, GAIN  When a frequency has its gain set to 1.0, the sound will be unaffected and represents the original signal exactly.
		{ 125.0f,	0.2f,	1.0f },
		{ 160.0f,	0.2f,	1.0f },
		{ 250.0f,	0.2f,	1.0f },
		{ 315.0f,	0.2f,	1.0f },
		{ 400.0f,	0.2f,	1.0f },
		{ 500.0f,	0.2f,	1.0f },
		{ 630.0f,	0.2f,	1.0f },
		{ 800.0f,	0.2f,	1.0f },
		{ 1000.0f,	0.2f,	1.0f },
		{ 1250.0f,	0.2f,	1.0f },
		{ 1600.0f,	0.2f,	1.0f },
		{ 2500.0f,	0.2f,	1.0f },
		{ 3150.0f,	0.2f,	1.0f },
		{ 4000.0f,	0.2f,	1.0f },
		{ 5000.0f,	0.2f,	1.0f },
		{ 6300.0f,	0.2f,	1.0f },
		{ 8000.0f,	0.2f,	1.0f },
		{ 10000.0f,	0.2f,	1.0f },
		{ 12500.0f,	0.2f,	1.0f },
		{ 16000.0f,	0.2f,	1.0f },
	};
	template< class InputIt, class OutputIt >
	OutputIt copy(InputIt first, InputIt last, OutputIt d_first);
};

#endif