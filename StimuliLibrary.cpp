#include "StimuliLibrary.h"
#include <cstdio>
#include <thread>			
#include <stdio.h>
#include <string.h>
//#include <unistd.h>		wyt todo
//#define LENGTH_OF_SELECTED_STIMULUS 0 // in the gui by selecting 0 we just play as long as the stimulus is
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
	return desiredDuration_ms;
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

StimuliLibrary::StimuliLibrary(): extradriverdata(nullptr), dFractionOfAudioFileLeftToPlay(0.00)
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

void StimuliLibrary::stop()
{
	printf("\n\n  Stimuli Stop - \n\n");
	early_stop = true;
	channel->setPaused(true);
	channel->stop();
}

bool StimuliLibrary::bLoadStimuli(int nr, float volume, unsigned int duration)
{
	bool bRetIsValidStimuli = true;
	channel->setMode(FMOD_DEFAULT); // reset to default
	desiredDuration_ms = duration;
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
			printf("\n\n Playing 01_chirp - no loop - \n\n");
			fsystem->createSound(pathToAudio_01Chirp.c_str(), FMOD_DEFAULT, 0, &audio);
			audio->getLength(&audioFileLength_ms, FMOD_TIMEUNIT_MS);
			channel->setChannelGroup(channelgroup);
			fsystem->playSound(audio, channelgroup, isPlaybackPaused, &channel);
			break;
		case 2:
			pathToCurrentAudioFile = pathToAudio_02PinkNoise;
			printf("\n\n Playing 02_pink - no loop - \n\n");
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
	//printf("\n audioFileLength_ms %i \n", audioFileLength_ms);
	//printf("\n desiredDuration_ms %f \n", desiredDuration_ms);
	// Case 1: Stimulus is shorter than desired length
	if (audioFileLength_ms < desiredDuration_ms)
	{
		
		unsigned int durationDifference_ms = desiredDuration_ms - audioFileLength_ms;
		double isLongerFactor = (double)desiredDuration_ms / (double)audioFileLength_ms; // e.g. 3.25
		unsigned int intMultiple = (int)isLongerFactor; // -> e.g. 3
		double isLongerFraction = isLongerFactor - (double)intMultiple; // -> e.g. 0.25

		//printf("\n\n (audioFileLength_ms < desiredDuration_ms) \n\n");
		//channel->setCallback(StimuliLibrary::EndOfSong);
		//printf("\intMultiple is %i \n", intMultiple);
		channel->setLoopCount(intMultiple - 1); // -1: as 1 Loop means playing the sound twice
		channel->setMode(FMOD_LOOP_NORMAL);
		channel->setPaused(false);
		// We do have some rest to play
		printf("\isLongerFactor is %f \n", isLongerFraction);
		dFractionOfAudioFileLeftToPlay = isLongerFraction;
	
		
		//printf("\ndFractionOfAudioFileLeftToPlay is %f \n", dFractionOfAudioFileLeftToPlay);
		desiredDuration_ms = isLongerFraction* (double)audioFileLength_ms; // For the call after the loop we only want to play the fraction that is left
		//printf("\n NEW desiredDuration_ms is %f \n", desiredDuration_ms);
	}
	// Case 2: Stimulus is longer than desired length
	else if (audioFileLength_ms > desiredDuration_ms)
	{
		printf("\n\n (audioFileLength_ms > desiredDuration_ms) \n\n");
		//unsigned int durationDifference_ms = (unsigned int) audioFileLength_ms - (unsigned int) desiredDuration_ms;
		//printf("\n durationDifference_ms %i \n", durationDifference_ms);


		// delay for ending 
		//FMOD_DELAYTYPE_END_MS myType;
		//channel->setDelay(FMOD_DELAYTYPE_END_MS,0,)





		/*
		audio->getLength(&audioFileLength_ms, FMOD_TIMEUNIT_MS);
		unsigned int audioLength_bytes;
		audio->getLength(&audioLength_bytes, FMOD_TIMEUNIT_RAWBYTES);
		// Get length of one byte
		double dMsLengthOfOneByte = (double)audioLength_bytes / (double)audioFileLength_ms;
		// Multipli with desired ms
		double dByteLengthOfDesiredMsLength = dMsLengthOfOneByte*desiredDuration_ms;
		// Cut the values
		int iByteLengthOfAudio = (int)dByteLengthOfDesiredMsLength;
		printf("\n raw byte length %i \n", audioLength_bytes);
		printf("\n to play byte length %i \n", iByteLengthOfAudio);

		// Create new audio file
		FMOD_CREATESOUNDEXINFO myAdditionalAudioSettings;
		memset(&myAdditionalAudioSettings, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		myAdditionalAudioSettings.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);             
		myAdditionalAudioSettings.length = iByteLengthOfAudio;
		myAdditionalAudioSettings.fileoffset = 0;
		float fFrequency;
		channel->getFrequency(&fFrequency);
		myAdditionalAudioSettings.defaultfrequency = fFrequency;
		myAdditionalAudioSettings.numchannels = 1;
		myAdditionalAudioSettings.format = FMOD_SOUND_FORMAT_BITSTREAM;
		bool isPlaybackPaused = true;

		printf("1 \n");
		fsystem->createSound(pathToCurrentAudioFile.c_str(), FMOD_OPENUSER, &myAdditionalAudioSettings, &audio);
		printf("2 \n");
		channel->setChannelGroup(channelgroup);
		printf("3 \n");
		fsystem->playSound(audio, channelgroup, isPlaybackPaused, &channel);
		printf("4 \n");
		*/
		channel->setPaused(false);
		vSetdFractionOfAudioFileLeftToPlay(0.00);
	}
	// Case 3: User piceked (perhaps accidently) a perfect match in durations
	else
	{
		printf("\n\n (Play full length stimulus) \n\n");
		channel->setPaused(false);
	}

	// TO DO (fischti)
	//priority
		//Default priority for the sound when played on a channel. 0 to 256. 0 = most important, 256 = least important.Default = 128.
	// -> Wenn sound kommt welcher den alten überschreibt -> setzte höhere Priorität (oder stoppe den alten)

	//early_stop = false;
	//std::thread t(timedStop,  channel, duration_stimuli);
	//t.detach();
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



