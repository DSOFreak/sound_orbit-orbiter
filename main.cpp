

#include "fcntl.h"
#include <chrono>
#include <queue>
#include <mutex>

#include <lirc/config.h>
#include <lirc/lirc_client.h>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <string.h>
#include "wiringPi.h"
#include "cmaxonmotor.h"
#include "fmod.hpp"

enum eTasten {
	KEY_1 = 1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,

	PLAY,
	STOP,
	NEXT,
	BACK,
	PREV,
	SUB_10,

	KEY_VOLUMEUP,
	KEY_VOLUMEDOWN,
	KEY_FORWARD, 
	KEY_BACKWARD,
	CHECK
};



#define DEBOUNCE 500000000;
#define WHEELPERI float(0.029 * 3.1415)
#define RAILPERI  float(2.2 * 3.1415)
using namespace std;

unsigned short iVoltage;
int iTargetPosition, iCurrentPosition, iNumbOffs, iAngle;
unsigned char cErrorNbr, cNumb[3];
CMaxonMotor * motor;
float vol = 0.5f;
float pitch;
int note;
FMOD::System    *fsystem;
FMOD::Channel   *channel1 = 0;
FMOD::Channel   *channel2 = 0;
FMOD::DSP       *dsp;
FMOD_RESULT      result;
unsigned int     version;
void            *extradriverdata = 0;

chrono::system_clock::time_point ttNow, ttOld;
chrono::system_clock::duration ttD;

char *szIRtxt;
char cTaste, old_cTaste;
string szTxt2, szTxt2_old;
int iLircSocket;
FMOD::Sound      *sound1;

std::queue<std::string> ir_queue;
std::mutex ir_mutex;


void ir_func() {
	char *code = new char;
	struct lirc_config *config;
	lirc_cmd_ctx ctx;

	//Initiate LIRC. Exit on failure 
	iLircSocket = lirc_init("irexec", 1);
	if (iLircSocket == -1)
		exit(EXIT_FAILURE);



	lirc_readconfig("/etc/lirc/RM-D420.conf", &config, NULL);
	usleep(1'000'000);
	while (1) {
		lirc_nextcode(&code);
		ir_mutex.lock();
		ir_queue.push(std::string(code));
		ir_mutex.unlock();
	}
}

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
// we have to use BCM numbering when initializing with wiringPiSetupSys
// when choosing a different pin number please use the BCM numbering, also
// update the Property Pages - Build Events - Remote Post-Build Event command 
// which uses gpio export for setup for wiringPiSetupSys
#define	LED	17
#define TaskTime 200


#define SOUNDS_DIR "/usr/lib/Orbiter/sounds"



int n_notes = 12;
std::string notes[] = { "A", "A#", "B", "C" , "C#", "D", "D#", "E", "F", "F#", "G", "G#" };
float pitches[] = { 1.0f,
1.0594630943592953f,
1.122462048309373f,
1.189207115002721f,
1.2599210498948732f,
1.3348398541700344f,
1.4142135623730951f,
1.4983070768766815f,
1.5874010519681994f,
1.681792830507429f,
1.7817974362806785f,
1.8877486253633868f };

float A_freqs[] = { 22.5f, 55.0f,110.0f,220.0f,440.0f,880.0f,1760.0f,3520.0f,7040.0f };

// Returns a pitch to a specific note
float getPitch(const int &octave, const std::string &note) {
	for (int i = 0; i < n_notes; i++) {
		if (notes[i].compare(note) == 0) return pitches[i] * A_freqs[octave];
	}
	return 0.0f;
}
// Returns a pitch to a specific note
float getPitch(const int &octave, const int &note) {
	if(note < n_notes) return pitches[note] * A_freqs[octave];
	return pitches[note % n_notes] * A_freqs[octave + (note / n_notes)];
}

void calcAll(void) {
	//iAngle = cNumb[2] + 10 * cNumb[1] + 100 * cNumb[0];
	//if (iAngle > 360) iAngle = 360;
	iTargetPosition = 65536 * float(iAngle) / 360.0 * RAILPERI / WHEELPERI;
	if (iNumbOffs<2) iNumbOffs++;
	else iNumbOffs = 0;
}



void IdleFunc(void) {
	
	motor->GetCurrentPosition(iCurrentPosition);
	motor->ErrorNbr(&cErrorNbr);
	if (motor->ErrorCode == 0x34000007) motor->initializeDevice();
	if (cErrorNbr != 0) motor->initializeDevice();
}

void TimerFunc(int value) {
	std::string s = "xx";
	bool ir_recived = false;
	ir_mutex.lock();
	while(!ir_queue.empty()){
		s = ir_queue.front();
		ir_queue.pop();
		ir_recived = true;
	}
	ir_mutex.unlock();

	ttOld = ttNow;
	ttNow = chrono::system_clock::now();
	ttD = (ttNow - ttOld);
	
	//if (ttD < (std::chrono::nanoseconds)200000000) return;
	
	szTxt2 = s;


	if (szTxt2.find("KEY_PLAY") != -1 /*&& cTaste != PLAY*/) {
		cTaste = PLAY;
	}
	if (szTxt2.find("check") != -1 /*&& cTaste != PLAY*/) {
		cTaste = CHECK;
	}
	if (szTxt2.find("KEY_STOP") != -1) {
		cTaste = STOP;
	}
	if (szTxt2.find("KEY_PREVIOUS") != -1 /*&& cTaste != PREV*/) {
		cTaste = PREV;
	}
	if (szTxt2.find("KEY_NEXT") != -1 /*&& cTaste != NEXT*/) {
		cTaste = NEXT;
	}
	if (szTxt2.find("KEY_1") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_1;
	}
	if (szTxt2.find("KEY_2") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_2;
	}
	if (szTxt2.find("KEY_3") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_3;
	}
	if (szTxt2.find("KEY_4") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_4;
	}
	if (szTxt2.find("KEY_5") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_5;
	}
	if (szTxt2.find("KEY_6") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_6;
	}
	if (szTxt2.find("KEY_7") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_7;
	}
	if (szTxt2.find("KEY_8") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_8;
	}
	if (szTxt2.find("KEY_9") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_9;
	}
	if (szTxt2.find("KEY_0") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_0;
	}
	if (szTxt2.find(">10") != -1 /*&& cTaste != NEXT*/) {
		cTaste = SUB_10;
	}


	if (old_cTaste != cTaste) { // Entprellen der Infrarot Fernsteuerung

		if (cTaste == CHECK) {

			std::string dir = std::string(SOUNDS_DIR) + "/wow/Wow " + std::to_string(static_cast<int>(rand()%30+1)) + ".wav";
			fsystem->createSound(dir.c_str(), FMOD_DEFAULT, 0, &sound1);
			fsystem->playSound(sound1, 0, false, &channel2);
		}
		if (szTxt2.find("KEY_PLAY") != -1 /*&& cTaste != PLAY*/) {
			
			motor->Move(iTargetPosition);
			//iAngle = cNumb[2] + 10 * cNumb[1] + 100 * cNumb[0];
			for (int iii = 0; iii <= 3; iii++) cNumb[iii] = 0;
			iNumbOffs = 0;
		}
		if (szTxt2.find("KEY_STOP") != -1) {
			motor->Halt();
			for (int iii = 0; iii <= 3; iii++) cNumb[iii] = 0;
			//iAngle = 0;
			//iNumbOffs = 0;

		}
		if (szTxt2.find("KEY_PREVIOUS") != -1 /*&& cTaste != PREV*/) {
			iAngle = iAngle - 15;
			calcAll();
		}
		if (szTxt2.find("KEY_NEXT") != -1 /*&& cTaste != NEXT*/) {
			iAngle = iAngle + 15;
			calcAll();
		}
		if (szTxt2.find("KEY_1") != -1 /*&& cTaste != NEXT*/) {
			cNumb[iNumbOffs] = 1;
			calcAll();
		}
		if (szTxt2.find("KEY_2") != -1 /*&& cTaste != NEXT*/) {
			cNumb[iNumbOffs] = 2;
			calcAll();
		}
		if (szTxt2.find("KEY_3") != -1 /*&& cTaste != NEXT*/) {
			cNumb[iNumbOffs] = 3;
			calcAll();
		}
		if (szTxt2.find("KEY_4") != -1 /*&& cTaste != NEXT*/) {
			cNumb[iNumbOffs] = 4;
			calcAll();
		}
		if (szTxt2.find("KEY_5") != -1 /*&& cTaste != NEXT*/) {
			cNumb[iNumbOffs] = 5;
			calcAll();
		}
		if (szTxt2.find("KEY_6") != -1 /*&& cTaste != NEXT*/) {
			cNumb[iNumbOffs] = 6;
			calcAll();
		}
		if (szTxt2.find("KEY_7") != -1 /*&& cTaste != NEXT*/) {
			cNumb[iNumbOffs] = 7;
			calcAll();
		}
		if (szTxt2.find("KEY_8") != -1 /*&& cTaste != NEXT*/) {
			cNumb[iNumbOffs] = 8;
			calcAll();
		}
		if (szTxt2.find("KEY_9") != -1 /*&& cTaste != NEXT*/) {
			cNumb[iNumbOffs] = 9;
			calcAll();
		}
		if (szTxt2.find("KEY_0") != -1 /*&& cTaste != NEXT*/) {
			cNumb[iNumbOffs] = 0;
			calcAll();
		}
		if (szTxt2.find(">10") != -1 /*&& cTaste != NEXT*/) {
			iTargetPosition = iTargetPosition - 1000;
		}
		old_cTaste = cTaste;

	}

	if (szTxt2.find("KEY_VOLUMEUP") != -1) {
		// increase volume
		cTaste = KEY_VOLUMEUP;
		if (vol < 0.99f) vol += 0.1f;
		channel1->setVolume(vol);
	}
	if (szTxt2.find("KEY_VOLUMEDOWN") != -1) {
		// decrease volume
		cTaste = KEY_VOLUMEDOWN;
		if (vol > 0.01f) vol -= 0.1f;
		channel1->setVolume(vol);
	}
	if (szTxt2.find("KEY_BACK") != -1) {
		// decrease pitch
		cTaste = KEY_BACKWARD;
		if (pitch > 44.0f) pitch = getPitch(0, --note);
		dsp->setParameterFloat(FMOD_DSP_OSCILLATOR_RATE, pitch);
	}
	if (szTxt2.find("KEY_FORWARD") != -1) { 
		// Increase pitch
		cTaste = KEY_FORWARD;
		if (pitch < 10000.0f) pitch = getPitch(0, ++note);
		dsp->setParameterFloat(FMOD_DSP_OSCILLATOR_RATE, pitch);
	}
	if (szTxt2.find("xx") != -1) {
		cTaste = 0;
	}
	
}
void		DisplayFunc(void)
{

	short iCurrent;


	//rectangle();

	motor->GetSupply(iVoltage, iCurrent);
	char szText[32];
	double dVoltage = double(4.25 * double(iVoltage) / 1000);
	

	printf( "Voltage: %2.2fV\n", dVoltage);

	printf( "Current: %d mA\n", iCurrent);

	printf( "Position: %d\n", iCurrentPosition);

	printf( "Target: %d\n", iTargetPosition);

	printf( "Error: %x\n", motor->ErrorCode);

	printf( "Error#: %x\n", cErrorNbr);

	printf( "Angle: %d\n", iAngle);

	printf("Pitch: %f\n", pitch);

	printf("Note: %d\n", note);

	printf("Volume: %f\n", vol);
	printf( "%f %f\n", RAILPERI, WHEELPERI);

	switch (cTaste) {
	case 0:break;
	case PLAY:printf("Run"); break;
	case STOP:printf("Stop"); break;
	case PREV:printf("PREV"); break;
	case NEXT:printf("NEXT"); break;
	case KEY_1:printf("001"); break;
	case KEY_2:printf("002"); break;
	case KEY_3:printf("003"); break;
	case KEY_4:printf("004"); break;
	case KEY_5:printf("005"); break;
	case KEY_6:printf("006"); break;
	case KEY_7:printf("007"); break;
	case KEY_8:printf("008"); break;
	case KEY_9:printf("009"); break;	
	case CHECK:printf("CHECK"); break;

	case KEY_FORWARD:printf("FORWARD"); break;
	case KEY_BACKWARD:printf("BACKWARD"); break;
	case KEY_VOLUMEUP:printf("VOLUMEUP"); break;
	case KEY_VOLUMEDOWN:printf("VOLUMEDOWN"); break;
	default: {}
	}
	printf("\n\n");
}
/* old glut function */
/*
void		KeyboardFunc(unsigned char key, int x, int y)
{
	// Function called when a key is hit
	//int foo;

	//foo = x + y; 
	// Has no effect: just to avoid a warning
	if ('q' == key || 'Q' == key || 27 == key)
		abort();
	if ('g' == key || 'G' == key)
		motor->Move(iTargetPosition);
	if ('s' == key || 'S' == key)
		motor->Halt();
	if ('v' == key || 'V' == key)
		iTargetPosition = 2000000;
	if ('r' == key || 'R' == key)
		iTargetPosition = -2000000;

	if ('t' == key || 'T' == key) {
		//strcpy(code, "aa");
		//char*  code_1;
		//lirc_nextcode(&code);

		//code = 10;
		//iTargetPosition = (aa+1)*1000;
	}
	if ('z' == key || 'Z' == key) {
		szIRtxt = "INIT";
	}

	glutPostRedisplay();
}
*/
int send_packet(lirc_cmd_ctx* ctx, int fd)
{
	int r;
	do {
		r = lirc_command_run(ctx, fd);
		if (r != 0 && r != EAGAIN)
			fprintf(stderr,
				"Error running command: %s\n", strerror(r));
	} while (r == EAGAIN);
	return r == 0 ? 0 : -1;
}

int main(int argc, char **argv)
{

	char InterfaceName[] = "USB0";
	motor = new CMaxonMotor(InterfaceName, 1);
	motor->initializeDevice(); // initialize EPOS2


	vol = 0.0f; // volume init
	note = 60;
	pitch = getPitch(0, note); //pitch init Musical note 'A5'
	
	// fmod init
	FMOD::System_Create(&fsystem);
	fsystem->getVersion(&version);

	if (version < FMOD_VERSION)
	{
		printf("FMOD lib version %08x doesn't match header version %08x \n", version, FMOD_VERSION);
		return -1;
	}
	
	fsystem->init(32, FMOD_INIT_NORMAL, extradriverdata);

	//Create an oscillator DSP units for the tone.
	fsystem->createDSPByType(FMOD_DSP_TYPE_OSCILLATOR, &dsp);
	dsp->setParameterFloat(FMOD_DSP_OSCILLATOR_RATE, pitch); 

	fsystem->playDSP(dsp, 0, true, &channel1);
	channel1->setVolume(0.0f);
	channel2->setVolume(1.0f);
	dsp->setParameterInt(FMOD_DSP_OSCILLATOR_TYPE, 0); // Set sinus oscillator
	channel1->setPaused(false);
	channel2->setPaused(false);

	//Load sound
	std::string dir = std::string(SOUNDS_DIR) + "/wow/Wow 1.wav";
	fsystem->createSound(dir.c_str(), FMOD_DEFAULT, 0, &sound1);
	sound1->setMode(FMOD_LOOP_OFF);

	cTaste = 0;


	
	std::thread ir_thread(ir_func);


	while (cTaste != SUB_10) {
		TimerFunc(0);
		DisplayFunc();
		IdleFunc();
		usleep(100000);
	}

	motor->closeDevice(); // close EPOS2

	delete motor;
	return 0;




	////wiringPiSetupSys();

	//pinMode(LED, OUTPUT);

	//while (true)
	//{
	//	digitalWrite(LED, HIGH);  // On
	//	delay(500); // ms
	//	digitalWrite(LED, LOW);	  // Off
	//	delay(500);
	//}

}