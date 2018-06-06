#pragma once
#include <fmod.hpp>
#include <queue>
#include <string>
#include <memory>
#include "MusicScale.h"
#include "cmaxonmotor.h"
#include "Toolbox.h"
using namespace std;
class StimuliLibrary
{
private:
	string pathToAudio_01Chirp = "../../../../TestFiles/01_chirp.flac";
	string pathToAudio_02PinkNoise = "../../../../TestFiles/02_pink192k.flac";
	string pathToAudio_03Sin500 = "../../../../TestFiles/03_sin500.flac";
	string pathToCurrentAudioFile;
	unsigned int audioFileLength_ms;
	unsigned int uiDesiredDuration_ms;

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
	std::queue<shared_ptr<Toolbox::HostData> > stimuli_queue;
	std::shared_ptr<Toolbox::HostData> hostDataOfHijackedProtocol;
	unsigned int uiGetDesiredStimuliDuration_ms();
	bool bGetIsThereAFractionLeftToPlay();
	void vSetdFractionOfAudioFileLeftToPlay(double dValue);
	void updateFSystem();
	StimuliLibrary();
	~StimuliLibrary();
	bool isFinished();
	void stopStimuli();
	bool bLoadStimuli(int nr, float volume, unsigned int duration);
	void playStimuli();
	void vPlayStimulusIfToBeTriggered();
	// Protocol hijacking to add specific non-time information for stimulus
	bool bAdaptStimulusParametersDueToHijacking(std::queue<shared_ptr<Toolbox::HostData>> movementQueue, CMaxonMotor* pMotor);
	//StimulusDuration(000000 - 999999) -> 6Digits in ms ~=15min max length for time framed stimulus
	static const int iPlayStimulusAsLongAsMovementsPending; // For example for the DSD360 test. Plays as long as there is something in the queue
	bool bIsStimulusToPlayAsLongAsMovementsPending();
	bool bCurrentlyAHijackedProtcolIsProcessed();
	void vSetHijackedProtocolIsCompletelyProcessed();
};

