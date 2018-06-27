#include "Equalizer.h"



Equalizer::Equalizer(FMOD::Channel* pChannel, FMOD::ChannelGroup* pChannelGroup, FMOD::System* pSystem, unsigned int uiSpeakerID, unsigned int uiStimulusNumber) : m_channel(pChannel),m_channelgroup(pChannelGroup),m_fsystem(pSystem), m_uiSpeakerID(uiSpeakerID), m_uiStimulusNumber(uiStimulusNumber)
{
	//DEBUG
	if (m_channel != pChannel)
	{
		printf(" (m_channel != pChannel) \n");
	}
	if (&m_channel != &pChannel)
	{
		printf(" (*m_channel != *pChannel) \n");
	}


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

void Equalizer::initDSPWithEQSettings()
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

	//Initialize all frequency bands
	//for (int i = 0; i < NUMBER_OF_TERCEBANDS; i++)
	//{
		//ersetze dsp_Eq_125Hz mit arrEqDSPObjects[i]

		// creates the channel group and gets the head DSP pointer
		res4 = m_fsystem->createChannelGroup("my_chan_grp", &m_channelgroup);
		res5 = m_channelgroup->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &pDSPChanGrpHead);
		res0 = m_fsystem->createDSPByType(FMOD_DSP_TYPE_PARAMEQ, &dsp_Eq_125Hz);
		res1 = dsp_Eq_125Hz->setParameterFloat(FMOD_DSP_PARAMEQ_CENTER, 4000.0f);
		res2 = dsp_Eq_125Hz->setParameterFloat(FMOD_DSP_PARAMEQ_BANDWIDTH, 0.2f);
		res3 = dsp_Eq_125Hz->setParameterFloat(FMOD_DSP_PARAMEQ_GAIN, -30.0f);
		res6 = pDSPChanGrpHead->addInput(dsp_Eq_125Hz, 0);

		// now we have to get the DSP Head pointer of the main channel and disconnect all existing DSP in order to avoid having our sound still playing without any modification
		res7 = m_channel->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &pDSPChanHead);
		res8 = pDSPChanHead->disconnectAll(false, true);
		res9 = dsp_Eq_125Hz->addInput(pDSPChanHead, 0);
		dsp_Eq_125Hz->setActive(true);


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
	//}
}
