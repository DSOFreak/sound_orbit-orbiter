#include "StimuliLibrary.h"
#include <cstdio>
#include <thread>			
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <limits>
using namespace std;
#define PLAY_STIMULUS_AS_LONG_AS_MOVEMENT_PENDING 991111 // own hijacking of protocol
const int StimuliLibrary::iPlayStimulusAsLongAsMovementsPending = PLAY_STIMULUS_AS_LONG_AS_MOVEMENT_PENDING;

//#include <unistd.h>		wyt todo

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
	//while(!early_stop && (time_ms >= 0))	wyt todo
	//{
	//	time_ms--;
	//	usleep(1000);
	//}
	//
	//channel->setPaused(true);
	//channel->stop();
}


unsigned int StimuliLibrary::uiGetDesiredStimuliDuration_ms()
{
	return uiDesiredDuration_ms;
}

bool StimuliLibrary::bGetIsThereAFractionLeftToPlay()
{
	//printf("dFractionOfAudioFileLeftToPlay %f", dFractionOfAudioFileLeftToPlay);

	if (dFractionOfAudioFileLeftToPlay > 0.0001)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void StimuliLibrary::vSetdFractionOfAudioFileLeftToPlay(double dValue)
{
	dFractionOfAudioFileLeftToPlay = (double)dValue;
}

void StimuliLibrary::updateFSystem()
{
	fsystem->update();

}

StimuliLibrary::StimuliLibrary(): extradriverdata(nullptr), dFractionOfAudioFileLeftToPlay(0.00), hostDataOfHijackedProtocol(nullptr)
{
	printf("StimuliLibrary constructor called \n");
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

bool StimuliLibrary::isFinished()
{
	bool val;
	channel->isPlaying(&val);
	return !val;
}

void StimuliLibrary::stopStimuli()
{

	printf("\n\n  Stimuli Stop - \n\n");
	audio->setMode(FMOD_LOOP_OFF);
	channel->setMode(FMOD_LOOP_OFF);


	early_stop = true;
	channel->setPaused(true);
	channel->stop();
	audio->release();
	fsystem->release();
	FMOD::System_Create(&fsystem);
	fsystem->getVersion(&version);
	if (version < FMOD_VERSION)
	{
		printf("FMOD lib version %08x doesn't match header version %08x \n", version, FMOD_VERSION);
	}
	fsystem->init(32, FMOD_INIT_NORMAL, nullptr);
}

bool StimuliLibrary::bLoadStimuli(int nr, float volume, unsigned int duration)
{
	//printf("\n\n  bLoadStimuli - \n\n");
	bool bRetIsValidStimuli = true;
	uiDesiredDuration_ms = duration;
	dsp_lowpass->setBypass(true);
	dFractionOfAudioFileLeftToPlay = 0.00;
	// Drin lassen für Referenz um sounds zu erzeugen
	if(nr >= 2 && nr <= 60)
	{
		float pitch = scale.getPitch(4, nr);
		dsp_sin->setParameterFloat(FMOD_DSP_OSCILLATOR_RATE, pitch);
		fsystem->playDSP(dsp_sin, 0, true, &channel);
	}
	bool isPlaybackPaused = true;
	switch(nr)
	{
		// TO do: Put stimuli in an array to improve speed. Indexed access (fischti)
		// - Create Object of each audio file at startup -> No need to do things like "getLength" more often

		//To load a sound into memory
		case 1:
			pathToCurrentAudioFile = pathToAudio_01Chirp;
			//printf("\n\n Playing 01_chirp - no loop - \n\n");
			fsystem->createSound(pathToAudio_01Chirp.c_str(), FMOD_DEFAULT, 0, &audio);
			audio->getLength(&audioFileLength_ms, FMOD_TIMEUNIT_MS);
			channel->setChannelGroup(channelgroup);
			fsystem->playSound(audio, channelgroup, isPlaybackPaused, &channel);
			break;
		case 2:
			pathToCurrentAudioFile = pathToAudio_02PinkNoise;
			//printf("\n\n Playing 02_pink - no loop - \n\n");
			fsystem->createSound(pathToAudio_02PinkNoise.c_str(), FMOD_DEFAULT, 0, &audio);
			audio->getLength(&audioFileLength_ms, FMOD_TIMEUNIT_MS);
			channel->setChannelGroup(channelgroup);
			fsystem->playSound(audio, channelgroup, isPlaybackPaused, &channel);
			break;
		case 3:
			pathToCurrentAudioFile = pathToAudio_03Sin500;
			//printf("\n\n Playing 03_sinus - no loop - \n\n");
			fsystem->createSound(pathToAudio_03Sin500.c_str(), FMOD_DEFAULT, 0, &audio);
			audio->getLength(&audioFileLength_ms, FMOD_TIMEUNIT_MS);
			channel->setChannelGroup(channelgroup);
			fsystem->playSound(audio, channelgroup, isPlaybackPaused, &channel);
			break;
		default:
			printf("\n\n Playing NOTHING - undefined stimulus number %i \n\n", nr);
			bRetIsValidStimuli = false;
			break;
	}

	channel->setVolume(volume);
	return bRetIsValidStimuli;
}

void StimuliLibrary::playStimuli()
{

	printf("\n playStimuli() \n");
	if (audioFileLength_ms == 0)
	{
		stopStimuli();
	}
	//printf("\n audioFileLength_ms %i \n", audioFileLength_ms);
	//printf("\n desiredDuration_ms %f \n", desiredDuration_ms);
	// Case 1: Stimulus is shorter than desired length
	else if (audioFileLength_ms < uiDesiredDuration_ms)
	{
		unsigned int durationDifference_ms = uiDesiredDuration_ms - audioFileLength_ms;
		double isLongerFactor = (double)uiDesiredDuration_ms / (double)audioFileLength_ms; // e.g. 3.25
		unsigned int intMultiple = (int)isLongerFactor; // -> e.g. 3
		double isLongerFraction = isLongerFactor - (double)intMultiple; // -> e.g. 0.25

		//printf("\n\n (audioFileLength_ms < desiredDuration_ms) \n\n");
		//channel->setCallback(StimuliLibrary::EndOfSong);
		//printf("\intMultiple is %i \n", intMultiple);
		channel->setLoopCount(intMultiple - 1); // -1: as 1 Loop means playing the sound twice
		channel->setMode(FMOD_LOOP_NORMAL);
		channel->setPaused(false);
		// We do have some rest to play
		//printf("\isLongerFactor is %f \n", isLongerFraction);
		dFractionOfAudioFileLeftToPlay = isLongerFraction;
		
		//printf("\ndFractionOfAudioFileLeftToPlay is %f \n", dFractionOfAudioFileLeftToPlay);
		uiDesiredDuration_ms = isLongerFraction* (double)audioFileLength_ms; // For the call after the loop we only want to play the fraction that is left
		//printf("\n NEW desiredDuration_ms is %f \n", desiredDuration_ms);
	}
	// Case 2: Stimulus is longer than desired length
	else if (audioFileLength_ms > uiDesiredDuration_ms)
	{
		printf("\n\n (audioFileLength_ms > desiredDuration_ms) \n\n");

		unsigned int audioFileLength_Samples;
		audio->getLength(&audioFileLength_Samples, FMOD_TIMEUNIT_PCM);
		// Get length of one sample
		double dMsLengthOfOneSample = (double)audioFileLength_Samples / (double)audioFileLength_ms;
		// Multipli with desired ms
		double dSampleLengthOfDesiredMsLength = dMsLengthOfOneSample*uiDesiredDuration_ms;
		// Cut the values
		int iSampleLengthOfAudio = (int)dSampleLengthOfDesiredMsLength;
		printf("audioFileLength_Samples: %i \n", audioFileLength_Samples);
		printf("we play samples: %i \n", iSampleLengthOfAudio);		

		fsystem->getMasterChannelGroup(&channelgroup);

		unsigned long long clockDSP, parentDSP;
		channelgroup->getDSPClock(&clockDSP, &parentDSP);
		unsigned int bufferLength = 2048; // som arbitrary vlue
		int numbuffers = 1;
		fsystem->getDSPBufferSize(&bufferLength, &numbuffers);
		fsystem->playSound(audio, channelgroup, true, &channel);
		FMOD_RESULT myResult = channelgroup->setDelay(clockDSP, clockDSP+ iSampleLengthOfAudio,true);

		channel->setPaused(false);
		vSetdFractionOfAudioFileLeftToPlay(0.00);
	}
	// Case 3: User piceked (perhaps accidently) a perfect match in durations
	else
	{
		//printf("\n\n (Play full length stimulus) \n\n");
		channel->setPaused(false);
	}

	// TO DO (fischti)
	//priority
		//Default priority for the sound when played on a channel. 0 to 256. 0 = most important, 256 = least important.Default = 128.
	// -> Wenn sound kommt welcher den alten überschreibt -> setzte höhere Priorität (oder stoppe den alten)

	channel->setPaused(false);

	//early_stop = false;
	//std::thread t(timedStop,  channel, duration_stimuli);
	//t.detach();
}

void StimuliLibrary::vPlayStimulusIfToBeTriggered()
{
	shared_ptr<Toolbox::HostData> hostData = stimuli_queue.front();
	stimuli_queue.pop(); // Delete the stimulus either way.

	if (hostData->toBeTriggerd == 1) // This is a check: Actually no stimulus should be in the queue which has not to be triggered.. makes no sense
	{
		bool bIsValidStimulus = bLoadStimuli(hostData->stimulus_nr, hostData->loudness, hostData->stimulusDuration);
		if (bIsValidStimulus)
		{
			playStimuli();
		}
	}
}

bool StimuliLibrary::bAdaptStimulusParametersDueToHijacking(std::queue<shared_ptr<Toolbox::HostData>> movementQueue, CMaxonMotor* pMotor)
{
	//printf("\n Going IN: bAdaptStimulusParametersDueToHijacking\n");
	bool bRetVal = false;
	bool isPaused = false;
	if (bCurrentlyAHijackedProtcolIsProcessed())
	{
		bRetVal = true;
	}
	// Check for all protocl hicjacking options (currently only one)
	if (bIsStimulusToPlayAsLongAsMovementsPending())
	{
		//printf("\n IsStimulusToPlayAsLongAsMovementsPending == True\n");
		if (!bCurrentlyAHijackedProtcolIsProcessed())
		{
			printf("\n bCurrentlyAHijackedProtcolIsProcessed\n");
			hostDataOfHijackedProtocol = stimuli_queue.front();
			stimuli_queue.pop(); // Delete the stimulus either way.
			bool bIsValidStimulus = bLoadStimuli(hostDataOfHijackedProtocol->stimulus_nr, hostDataOfHijackedProtocol->loudness, hostDataOfHijackedProtocol->stimulusDuration);
		}

		if (!movementQueue.empty() || !pMotor->reachedTarget())
		{
			if (hostDataOfHijackedProtocol->toBeTriggerd == 1) // This is a check: Actually no stimulus should be in the queue which has not to be triggered.. makes no sense
			{	
				printf("\n Tryining infintie play\n");
				channel->setMode(FMOD_LOOP_NORMAL);
				channel->setLoopCount(-1); // -1: is infinite

				channel->setPaused(false);
				hostDataOfHijackedProtocol->toBeTriggerd = 0;
			}
		}
		else if (pMotor->reachedTarget())
		{
			printf("\nAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
			
			vSetHijackedProtocolIsCompletelyProcessed();
			stopStimuli();
		}
		bRetVal =  true;
	}
	else
	{
		bRetVal = false;
	}
	//printf("\n Going OUT: bAdaptStimulusParametersDueToHijacking\n");
	return bRetVal;
}



bool StimuliLibrary::bIsStimulusToPlayAsLongAsMovementsPending()
{
	
	bool bIsValidStimulus;
	if (bCurrentlyAHijackedProtcolIsProcessed())
	{
		bIsValidStimulus = bLoadStimuli(hostDataOfHijackedProtocol->stimulus_nr, hostDataOfHijackedProtocol->loudness, hostDataOfHijackedProtocol->stimulusDuration);
		if (bIsValidStimulus)
		{
			//printf("\nbIsStimulusToPlayAsLongAsMovementsPending bIsValidStimulus\n");
			if (uiDesiredDuration_ms == iPlayStimulusAsLongAsMovementsPending)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		if (!stimuli_queue.empty())
		{
			shared_ptr<Toolbox::HostData> hostData = stimuli_queue.front();
			//printf("hostData->stimulusDuration %i \n", hostData->stimulusDuration);
			bool bIsValidStimulus = bLoadStimuli(hostData->stimulus_nr, hostData->loudness, hostData->stimulusDuration);
			//printf("DESIRED DURATION %i \n", uiDesiredDuration_ms);
			//printf("iPlayStimulusAsLongAsMovementsPending %i \n", iPlayStimulusAsLongAsMovementsPending);
			if (uiDesiredDuration_ms == iPlayStimulusAsLongAsMovementsPending)
			{
				//printf("\n desiredDuration_ms == iPlayStimulusAsLongAsMovementsPending\n");
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			//printf("\n stimuli_queue.empty() \n");
			return false;
		}
	}
}

bool StimuliLibrary::bCurrentlyAHijackedProtcolIsProcessed()
{
	if (hostDataOfHijackedProtocol != nullptr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void StimuliLibrary::vSetHijackedProtocolIsCompletelyProcessed()
{
	hostDataOfHijackedProtocol = nullptr;
}


FMOD_RESULT F_CALLBACK StimuliLibrary::EndOfSong(FMOD_CHANNELCONTROL*channelControl, FMOD_CHANNELCONTROL_TYPE controlType,
	FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void*commanData1, void*commanData2)
{
	if (controlType == FMOD_CHANNELCONTROL_CHANNEL&&callbackType == FMOD_CHANNELCONTROL_CALLBACK_END) {
		
		printf("\n\n CALLBACK \n\n");




		FMOD::Channel*channel = (FMOD::Channel*)channelControl;
	}


	return FMOD_OK;
}



