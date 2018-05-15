#pragma once
#include <fmod.hpp>
#include <string>
#include "MusicScale.h"

class StimuliLibrary
{
private:
	FMOD::System    *fsystem;
	FMOD::Channel   *channel;
	FMOD::DSP       *dsp_sin;
	FMOD::DSP       *dsp_noise;
	FMOD::DSP		*dsp_lowpass;
	FMOD_RESULT      result;
	unsigned int version;

	MusicScale scale;

	void *extradriverdata;
	unsigned int duration_stimuli;
	bool early_stop;
	void initAllStimuli();
	static void timedStop(FMOD::Channel* channel, unsigned time_ms);


public:
	StimuliLibrary();
	~StimuliLibrary();
	void loadStimuli(int nr, float volume, unsigned duration);
	bool isFinished();
	void stop();
	void playStimuli();
};

