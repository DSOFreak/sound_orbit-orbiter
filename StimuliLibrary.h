#ifndef STIMULILIB_H
#define STIMULILIB_H
#include "Equalizer.h"
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
	// Equalizer objects
	std::shared_ptr<Equalizer> pEqSpeakerWN;
	std::shared_ptr<Equalizer> pEqSpeakerPN;

	// Audio files
	string pathToAudio_01WhiteNoise = "../../../../TestFiles/WN_N24_FS48k_Flanke10ms_6min.flac";
	string pathToAudio_02PinkNoise = "../../../../TestFiles/PN_N24_FS48k_Flanke10ms_6min.flac";
	string pathToAudio_03Sin500 = "../../../../TestFiles/03_sin500.flac";
	string pathToCurrentAudioFile; //deprecated
	unsigned int audioFileLength_ms;
	unsigned int uiDesiredDuration_ms;

	// If audioFileLength_ms<desiredDuration_ms and we have a fraction of the audiofile left to play
	double dFractionOfAudioFileLeftToPlay;

	FMOD::System    *fsystem;
	FMOD::Channel   *channel;
	FMOD::ChannelGroup *channelgroup;
	FMOD::Sound		*audio;
	FMOD::Sound		*audio_Stimulus1;
	FMOD::Sound		*audio_Stimulus2;
	FMOD::Sound		*audio_Stimulus3;
	FMOD::Channel   *channel_Stimulus1;
	FMOD::Channel   *channel_Stimulus2;
	FMOD::Channel   *channel_Stimulus3;
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


	//debug
	/*FMOD::DSP *pDSPChanHead;
	FMOD::DSP *pDSPChanGrpHead;
	FMOD::DSP	*dsp_Eq_125Hz;*/
	void vSetChannelToInvinitePlay();
public:
	FMOD::Channel* getChannel();
	FMOD::ChannelGroup* getChannelGroup();
	FMOD::System * getSystem();

	std::queue<shared_ptr<Toolbox::HostData> > stimuli_queue;
	std::shared_ptr<Toolbox::HostData> hostDataOfHijackedProtocol;
	unsigned int uiGetDesiredStimuliDuration_ms();
	bool bGetIsThereAFractionLeftToPlay();
	void vSetdFractionOfAudioFileLeftToPlay(double dValue);
	void updateFSystem();
	void initEqualizers();
	StimuliLibrary();
	~StimuliLibrary();
	bool isFinished();
	void stopStimuli();
	bool bLoadStimuli(int nr, float volume, unsigned int duration);
	void playStimuli();
	void vPlayStimulusIfToBeTriggered();
	// Protocol hijacking to add specific non-time information for stimulus
	bool bAdaptStimulusParametersDueToHijacking(std::queue<shared_ptr<Toolbox::HostData>> &movementQueue, std::shared_ptr<CMaxonMotor> pMotor);
	//StimulusDuration(000000 - 999999) -> 6Digits in ms ~=15min max length for time framed stimulus
	static const int iPlayStimulusAsLongAsMovementsPending; // For example for the DSD360 test. Plays as long as there is something in the queue
	bool bIsStimulusToPlayAsLongAsMovementsPending();
	bool bCurrentlyAHijackedProtcolIsProcessed();
	void vSetHijackedProtocolIsCompletelyProcessed();

};

#endif