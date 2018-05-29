#pragma once
#include <fmod.hpp>
#include <string>
#include "MusicScale.h"
using namespace std;
class StimuliLibrary
{
private:
	string pathToAudio_01Chirp = "../../../../TestFiles/01_chirp.flac";
	string pathToAudio_02PinkNoise = "../../../../TestFiles/02_pink192k.flac";
	string pathToAudio_03Sin500 = "../../../../TestFiles/03_sin500.flac";

	FMOD::System    *fsystem;
	FMOD::Channel   *channel;
	FMOD::ChannelGroup *channelgroup;
	FMOD::Sound		*audio;
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

