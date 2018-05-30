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
	string pathToCurrentAudioFile;
	unsigned int audioFileLength_ms;
	double desiredDuration_ms;

	// If audioFileLength_ms<desiredDuration_ms and we have a fraction of the audiofile left to play
	double dFractionOfAudioFileLeftToPlay;

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
	bool early_stop;
	void initAllStimuli();
	static void timedStop(FMOD::Channel* channel, unsigned time_ms);



	static FMOD_RESULT F_CALLBACK EndOfSong(FMOD_CHANNELCONTROL*channelControl, FMOD_CHANNELCONTROL_TYPE controlType,
		FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void*commanData1, void*commanData2);

public:
	unsigned int uiGetDesiredStimuliDuration_ms();
	bool bGetIsThereAFractionLeftToPlay();
	void vSetdFractionOfAudioFileLeftToPlay(double dValue);
	void updateFSystem();
	StimuliLibrary();
	~StimuliLibrary();
	bool isFinished();
	void stop();
	bool bLoadStimuli(int nr, float volume, unsigned int duration);
	void playStimuli();
};

