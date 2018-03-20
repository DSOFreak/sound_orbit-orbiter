#include "StimuliLibrary.h"
#include <cstdio>
#include <thread>
#include <unistd.h>

void StimuliLibrary::initAllStimuli()
{
	//Stimuli 1:
	fsystem->createDSPByType(FMOD_DSP_TYPE_OSCILLATOR, &dsp_sin);
	fsystem->createDSPByType(FMOD_DSP_TYPE_OSCILLATOR, &dsp_noise);
	fsystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &dsp_lowpass);
	dsp_sin->setParameterInt(FMOD_DSP_OSCILLATOR_TYPE, 0);
	dsp_noise->setParameterInt(FMOD_DSP_OSCILLATOR_TYPE, 5);
	channel->addDSP(0, dsp_lowpass);
	dsp_lowpass->setBypass(true);
}

void StimuliLibrary::timedStop(FMOD::Channel *channel ,unsigned int time_ms)
{
	usleep(time_ms * 1000);
	channel->setPaused(true);
	channel->stop();
}

StimuliLibrary::StimuliLibrary(): extradriverdata(nullptr)
{
	FMOD::System_Create(&fsystem);
	fsystem->getVersion(&version);

	if (version < FMOD_VERSION)
	{
		printf("FMOD lib version %08x doesn't match header version %08x \n", version, FMOD_VERSION);
	}

	fsystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
	

	initAllStimuli();
}


StimuliLibrary::~StimuliLibrary()
{

}

void StimuliLibrary::loadStimuli(int nr, float volume, unsigned int duration)
{
	duration_stimuli = duration;
	dsp_lowpass->setBypass(true);
	
	if(nr >= 2 && nr <= 60)
	{
		float pitch = scale.getPitch(4, nr);
		dsp_sin->setParameterFloat(FMOD_DSP_OSCILLATOR_RATE, pitch);
		fsystem->playDSP(dsp_sin, 0, true, &channel);
	}
	switch(nr)
	{
		case 0:
			// White noise
			fsystem->playDSP(dsp_noise, 0, true, &channel);
			break;		
		case 1:
			// Pink noise
			fsystem->playDSP(dsp_noise, 0, true, &channel);
			dsp_lowpass->setBypass(false);
			break;
	}
	channel->setVolume(volume);
}

void StimuliLibrary::playStimuli()
{
	channel->setPaused(false);
	std::thread t(timedStop,  channel, duration_stimuli);
	t.detach();
}


