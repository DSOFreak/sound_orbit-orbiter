#include "Equalizer.h"



Equalizer::Equalizer( unsigned int uiSpeakerID, unsigned int uiStimulusNumber) : m_uiSpeakerID(uiSpeakerID), m_uiStimulusNumber(uiStimulusNumber)
{



	// Get correct eq settings
	switch (uiSpeakerID)
	{
		case 0: 
			if (uiStimulusNumber == 1) // WN
			{
				std::copy(&arrfSpeaker0WN[0][0], &arrfSpeaker0WN[0][0] + NUMBER_OF_TERCEBANDS*NUMBER_OF_STIMULI, &myEqSettings[0][0]);
			}
			else if (uiStimulusNumber == 2) // PN
			{
				std::copy(&arrfSpeaker0PN[0][0], &arrfSpeaker0PN[0][0] + NUMBER_OF_TERCEBANDS*NUMBER_OF_STIMULI, &myEqSettings[0][0]);
				//Debug
				/*
				float fCenterFreq = myEqSettings[0][0];
				float fBandwidth = myEqSettings[0][1];
				float fGain = myEqSettings[0][2];
				printf("fCenterFreq (%f)\n", fCenterFreq);
				printf("fBandwidth (%f)\n", fBandwidth);
				printf("fGain! (%f)\n", fGain);*/
			}
			else
			{
				
			}
		/*case 1:
			if (uiStimulusNumber == 1)
			{
				myEqSettings =
			}
			else if (uiStimulusNumber == 2)
			{
				myEqSettings = arrfSpeaker1PN;
			}
		case 2:
			if (uiStimulusNumber == 1)
			{
				myEqSettings =
			}
			else if (uiStimulusNumber == 2)
			{
				myEqSettings = arrfSpeaker2PN;
			}*/
		default:
			break;
	};

}

Equalizer::~Equalizer()
{
}

void Equalizer::initDSPWithEQSettings(FMOD::Channel* pChannel, FMOD::ChannelGroup* pChannelGroup, FMOD::System* pSystem)
{
	//Debug
	FMOD_RESULT res0;
	FMOD_RESULT res1;
	FMOD_RESULT res2;
	FMOD_RESULT res3;
	FMOD_RESULT res4;
	FMOD_RESULT res5;
	FMOD_RESULT res6;
	FMOD_RESULT res7;
	FMOD_RESULT res8;
	FMOD_RESULT res9;
	FMOD_RESULT res1337;
	//Initialize all frequency bands
	float fCenterFreq;
	float fBandwidth;
	float fGain;
	res4 = pSystem->createChannelGroup("my_chan_grp", &pChannelGroup);
	res5 = pChannelGroup->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &pDSPChanGrpHead);
	for (int i = 0; i < NUMBER_OF_TERCEBANDS; i++)
	//for (int i = 0; i < 2; i++)
	{ 
		res0 = pSystem->createDSPByType(FMOD_DSP_TYPE_PARAMEQ, &arrEqDSPObjects[i]);
		fCenterFreq = myEqSettings[i][0];
		fBandwidth = myEqSettings[i][1];
		fGain = myEqSettings[i][2];
		//printf("fCenterFreq (%f)\n", fCenterFreq);
		//printf("fBandwidth (%f)\n", fBandwidth);
		//printf("fGain! (%f)\n", fGain);
		res1 = arrEqDSPObjects[i]->setParameterFloat(FMOD_DSP_PARAMEQ_CENTER, fCenterFreq);
		res2 = arrEqDSPObjects[i]->setParameterFloat(FMOD_DSP_PARAMEQ_BANDWIDTH, fBandwidth);
		res3 = arrEqDSPObjects[i]->setParameterFloat(FMOD_DSP_PARAMEQ_GAIN, fGain);
		

		if (i == 0)
		{
			res6 = pDSPChanGrpHead->addInput(arrEqDSPObjects[i], 0);
		}	
		else
		{
			res1337 = arrEqDSPObjects[i-1]->addInput(arrEqDSPObjects[i], 0); // 
		}
		if (res1337 != FMOD_OK)
		{
			printf("FMOD error res1337! (%d)\n", res1337);
			exit(-1);
		}
		if (res0 != FMOD_OK)
		{
			printf("FMOD error res0! (%d)\n", res0);
			exit(-1);
		}
		if (res1 != FMOD_OK)
		{
			printf("FMOD error res1! (%d)\n", res1);
			exit(-1);
		}
		if (res2 != FMOD_OK)
		{
			printf("FMOD error res2! (%d)\n", res2);
			exit(-1);
		}
		if (res3 != FMOD_OK)
		{
			printf("FMOD error res3! (%d)\n", res3);
			exit(-1);
		}
		if (res4 != FMOD_OK)
		{
			printf("FMOD error res4! (%d)\n", res4);
			exit(-1);
		}
		if (res5 != FMOD_OK)
		{
			printf("FMOD error res5! (%d)\n", res5);
			exit(-1);
		}
		if (res6 != FMOD_OK)
		{
			printf("FMOD error res6! (%d)\n", res6);
			exit(-1);
		}

	}
	// now we have to get the DSP Head pointer of the main channel and disconnect all existing DSP in order to avoid having our sound still playing without any modification
	res7 = pChannel->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &pDSPChanHead);
	res8 = pDSPChanHead->disconnectAll(false, true);

	res9 = arrEqDSPObjects[NUMBER_OF_TERCEBANDS-1]->addInput(pDSPChanHead, 0);//DAS IST DEBUG
	
	//res9 = arrEqDSPObjects[NUMBER_OF_TERCEBANDS-1]->addInput(pDSPChanHead, 0); DAS HIER MUSS REIN
	for (int k = 0; k < NUMBER_OF_TERCEBANDS; k++)
	{
		arrEqDSPObjects[k]->setActive(true);
		if (res7 != FMOD_OK)
		{
			printf("FMOD error res7! (%d)\n", res6);
			exit(-1);
		}
		if (res8 != FMOD_OK)
		{
			printf("FMOD error res8! (%d)\n", res6);
			exit(-1);
		}
		if (res9 != FMOD_OK)
		{
			printf("FMOD error res9! (%d)\n", res6);
			exit(-1);
		}
	}

}
