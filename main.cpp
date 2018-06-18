

#include "fcntl.h"
#include <chrono>
#include <queue>
#include <mutex>
#include <csignal>
#include <lirc/config.h>
#include <lirc/lirc_client.h>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <string.h>
#include "wiringPi.h"
#include "cmaxonmotor.h"
#include "fmod.hpp"
#include "TCPClient.h"
#include "Toolbox.h"
#include "StimuliLibrary.h" 

using namespace std;

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

std::queue<shared_ptr<Toolbox::HostData> > movement_queue;

bool exit_app;
chrono::system_clock::time_point ttNow, ttOld;
chrono::system_clock::duration ttD;

char *szIRtxt;
char cTaste, old_cTaste;
string szTxt2, szTxt2_old;
int iLircSocket;

std::queue<std::string> ir_queue;
std::mutex ir_mutex;

std::queue<std::string> tcp_queue;
std::mutex tcp_mutex;
TCPClient tcp;
char *ip_addr;
int port;

StimuliLibrary stimuliLib;


void ir_func() {
	char *code = new char;
	struct lirc_config *config;
	lirc_cmd_ctx ctx;

	//Initiate LIRC. Exit on failure 
	iLircSocket = lirc_init("irexec", 1);
	if (iLircSocket == -1)
		exit(EXIT_FAILURE);



	lirc_readconfig("/etc/lirc/RM-D420.conf", &config, NULL);
	usleep(1000000);
	while (!exit_app) {
		lirc_nextcode(&code);
		ir_mutex.lock();
		ir_queue.push(std::string(code));
		ir_mutex.unlock();
	}
}

void tcp_func() {
	std::string msg;

	while (!exit_app) {
		msg = tcp.receive(100);
		if (msg.length() == 0) {
			tcp.exit();
			std::cout << "Reconnecting..." << std::endl;

			tcp.setup(ip_addr, port);
			sleep(2);
		}
		tcp_mutex.lock();
		tcp_queue.push(msg);
		tcp_mutex.unlock();
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



void calcTargetPosition(void) {
	//iAngle = cNumb[2] + 10 * cNumb[1] + 100 * cNumb[0];
	//if (iAngle > 360) iAngle = 360;
	iTargetPosition = 65536 * float(iAngle) / 360.0 * RAILPERI / WHEELPERI;
	if (iNumbOffs<2) iNumbOffs++;
	else iNumbOffs = 0;
}



void IdleFunc(void) {
	motor->getCurrentPosition(iCurrentPosition);
	motor->ErrorNbr(&cErrorNbr);
	if (motor->ErrorCode == 0x34000007) motor->initializeDevice();
	if (cErrorNbr != 0) motor->initializeDevice();
}

bool movement_skip;
bool stimuli_skip;
void vProcessMovement()
{
	shared_ptr<Toolbox::HostData> hostData = movement_queue.front();
	movement_queue.pop();

	std::cout << "Excecuting command:" << std::endl;
	std::cout << "hostData: " << "dir = " << static_cast<int>(hostData->direction) << ", angleToMove = " << hostData->angularDistance << ", speed = " << hostData->speed << std::endl;
	std::cout << "stim_nr = " << static_cast<int>(hostData->stimulus_nr) << ", stim_dur = " << hostData->stimulusDuration << ", vol = " << hostData->loudness << ", toBeTriggerd = " << hostData->toBeTriggerd << std::endl;

	if (hostData->direction == 1) { // Dir 1 = clockwise
		iAngle = hostData->angularDistance; // Correct
	}
	if (hostData->direction == 2) { // Dir 2 = counterclockwise
		iAngle = hostData->angularDistance * -1;
	}
	if (hostData->direction != 0) { // Dir 0 = no movement
		calcTargetPosition();
		motor->setSpeed(hostData->speed);
		motor->Move(iTargetPosition);
	}
}
void TimerFunc(bool& bIsFirstCall) {
	std::string s = "xx";
	bool ir_recived = false;
	ir_mutex.lock();
	while (!ir_queue.empty()) {
		s = ir_queue.front();
		ir_queue.pop();
		ir_recived = true;
	}
	ir_mutex.unlock();


	std::string host_data_raw;
	while (!tcp_queue.empty()) {
		tcp_mutex.lock();
		host_data_raw = tcp_queue.front(); // Get tcp messages
		tcp_queue.pop();
		tcp_mutex.unlock();

	}

	/* PROTOCOL INTERPRETATION */
	if (host_data_raw.length() != 0) { // If a tcp-message has arrived
		std::cout << "raw hostData: " << host_data_raw << std::endl;

		shared_ptr<Toolbox::HostData> hostData(new Toolbox::HostData(Toolbox::decodeHostData(host_data_raw, 0))); // decode host data
		if (hostData->mov_queued) { // Add new data to queue
			std::cout << "Add new data to queue" << endl;
			movement_queue.push(hostData);
			movement_skip = false;
		}
		else { // Clear Queue and Add new Data to Queue
			std::cout << " Clear Queue and Add new Data to Queue" << endl;
			movement_skip = true;
			while (!movement_queue.empty())
			{
				movement_queue.pop();
			}
			movement_queue.push(hostData);
		}
		if (hostData->stim_queued) { // Add new data to queue
			stimuliLib.stimuli_queue.push(hostData);
			stimuli_skip = false;
		}
		else { // Clear Queue and Add new Data to Queue
			stimuli_skip = true;
			while (!stimuliLib.stimuli_queue.empty())
			{
				stimuliLib.stimuli_queue.pop();
			}
			stimuliLib.stimuli_queue.push(hostData);
		}
	}


	/* MOVEMENT PROCESSING */
	if (!movement_queue.empty())// movement pending 
	{
		if (motor->reachedTarget() || movement_skip) // (movementFinnished OR Skip_this_movement)
		{
			vProcessMovement();
		}
		// Here we ware if we have some movements in our queue which we want to do but still other movements are going on
		else if (bIsFirstCall == true)
		{
			bIsFirstCall = false;
			cout << "Movement due to first call" << endl;
			vProcessMovement();
			// is Motor moving? else: process further movement
			//cout << "++++++++++++++++++++++++++Motor not in position we wait for movement to finish" << endl;
			//vProcessMovement();
		}
	}

	/* STIMULI PROCESSING */

	stimuliLib.updateFSystem();
	// Check if there is a protocol hicjacking
	if (!stimuliLib.bAdaptStimulusParametersDueToHijacking(movement_queue,motor)) // not protocl adaption, process as usual
	{
		//cout << "No Hijacking" << endl;
		if (stimuliLib.bGetIsThereAFractionLeftToPlay())
		{
			//printf("\n We have a fraction left to play of %d milliseconds\n", stimuliLib.uiGetDesiredStimuliDuration_ms());
			stimuliLib.playStimuli(); // Enter here only if (audioFileLength_ms < desiredDuration_ms)
		}
		else if (!stimuliLib.stimuli_queue.empty())
		{
			//cout << "We try to play a stimulus if it has to be triggered" << endl;
			stimuliLib.vPlayStimulusIfToBeTriggered();
		}
	}	

		
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++INFRARED PROCESSING +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	szTxt2 = s;


	if (szTxt2.find("KEY_PLAY") != -1  ) {
		cTaste = PLAY;
	}
	if (szTxt2.find("check") != -1  ) {
		cTaste = CHECK;
	}
	if (szTxt2.find("KEY_STOP") != -1) {
		cTaste = STOP;
	}
	if (szTxt2.find("KEY_PREVIOUS") != -1) {
		cTaste = PREV;
	}
	if (szTxt2.find("KEY_NEXT") != -1  ) {
		cTaste = NEXT;
	}
	if (szTxt2.find("KEY_1") != -1  ) {
		cTaste = KEY_1;
	}
	if (szTxt2.find("KEY_2") != -1  ) {
		cTaste = KEY_2;
	}
	if (szTxt2.find("KEY_3") != -1  ) {
		cTaste = KEY_3;
	}
	if (szTxt2.find("KEY_4") != -1  ) {
		cTaste = KEY_4;
	}
	if (szTxt2.find("KEY_5") != -1  ) {
		cTaste = KEY_5;
	}
	if (szTxt2.find("KEY_6") != -1  ) {
		cTaste = KEY_6;
	}
	if (szTxt2.find("KEY_7") != -1  ) {
		cTaste = KEY_7;
	}
	if (szTxt2.find("KEY_8") != -1  ) {
		cTaste = KEY_8;
	}
	if (szTxt2.find("KEY_9") != -1  ) {
		cTaste = KEY_9;
	}
	if (szTxt2.find("KEY_0") != -1  ) {
		cTaste = KEY_0;
	}
	if (szTxt2.find(">10") != -1  ) {
		cTaste = SUB_10;
	}
	if (szTxt2.find("KEY_VOLUMEUP") != -1) {
		cTaste = KEY_VOLUMEUP;
	}
	if (szTxt2.find("KEY_VOLUMEDOWN") != -1) {
		cTaste = KEY_VOLUMEDOWN;
	}
	if (szTxt2.find("KEY_BACK") != -1) {
		cTaste = KEY_BACKWARD;
	}
	if (szTxt2.find("KEY_FORWARD") != -1) {
		cTaste = KEY_FORWARD;
	}
	if (szTxt2.find("xx") != -1) {
		cTaste = 0;
	}

	if (old_cTaste != cTaste) { // Entprellen der Infrarot Fernsteuerung

		if (cTaste == CHECK) {
			/*
			std::string dir = std::string(SOUNDS_DIR) + "/wow/Wow " + std::to_string(static_cast<int>(rand()%30+1)) + ".wav";
			fsystem->createSound(dir.c_str(), FMOD_DEFAULT, 0, &sound1);
			fsystem->playSound(sound1, 0, false, &channel2);*/
		}
		if (cTaste == PLAY) {
			
			motor->Move(iTargetPosition);
			//iAngle = cNumb[2] + 10 * cNumb[1] + 100 * cNumb[0];
			for (int iii = 0; iii <= 3; iii++) cNumb[iii] = 0;
			iNumbOffs = 0;
		}
		if (cTaste == STOP) {
			motor->Halt();
			for (int iii = 0; iii <= 3; iii++) cNumb[iii] = 0;
			//iAngle = 0;
			//iNumbOffs = 0;

		}
		if (cTaste == PREV) {
			iAngle = iAngle - 15;
			calcTargetPosition();
		}
		if (cTaste == NEXT) {
			iAngle = iAngle + 15;
			calcTargetPosition();
		}
		if (cTaste == KEY_1) {
			cNumb[iNumbOffs] = 1;
			calcTargetPosition();
		}
		if (cTaste == KEY_2) {
			cNumb[iNumbOffs] = 2;
			calcTargetPosition();
		}
		if (cTaste == KEY_3) {
			cNumb[iNumbOffs] = 3;
			calcTargetPosition();
		}
		if (cTaste == KEY_4) {
			cNumb[iNumbOffs] = 4;
			calcTargetPosition();
		}
		if (cTaste == KEY_5) {
			cNumb[iNumbOffs] = 5;
			calcTargetPosition();
		}
		if (cTaste == KEY_6) {
			cNumb[iNumbOffs] = 6;
			calcTargetPosition();
		}
		if (cTaste == KEY_7) {
			cNumb[iNumbOffs] = 7;
			calcTargetPosition();
		}
		if (cTaste == KEY_8) {
			cNumb[iNumbOffs] = 8;
			calcTargetPosition();
		}
		if (cTaste == KEY_9) {
			cNumb[iNumbOffs] = 9;
			calcTargetPosition();
		}
		if (cTaste == KEY_0) {
			cNumb[iNumbOffs] = 0;
			calcTargetPosition();
		}
		if (cTaste == SUB_10) {
			iTargetPosition = iTargetPosition + 100000;
		}
		old_cTaste = cTaste;

	}

	if (cTaste == KEY_VOLUMEUP) {
		// increase volume
	}
	if (cTaste == KEY_VOLUMEDOWN) {
		// decrease volume
	}
	if (cTaste == KEY_BACKWARD) {
		// decrease pitch
	}
	if (cTaste == KEY_FORWARD) {
	}
	if (cTaste == 0) {

	}
	
}
void		DisplayFunc(void)
{

	short iCurrent;


	//rectangle();

	motor->GetSupply(iVoltage, iCurrent);
	char szText[32];
	double dVoltage = double(4.25 * double(iVoltage) / 1000);
	
	/*
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
	*/
	/*
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
	*/
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


namespace
{
	volatile std::sig_atomic_t gSignalStatus;
}

void signal_handler(int signal)
{
	gSignalStatus = signal;
	std::cout << "SIGTERM RECEIVED... Shutting down."<< std::endl;
	exit_app = true;
}



int main(int argc, char **argv)
{

	// argv[1] = Ip
	

	printf("Starting Orbiter Program.");
	char InterfaceName[] = "USB0";
	motor = new CMaxonMotor(InterfaceName, 1);
	motor->initializeDevice(); // initialize EPOS2


	cTaste = 0;


	
	std::thread ir_thread(ir_func);	

	std::thread tcp_thread(tcp_func);
	if (argc == 1)
	{
		printf("No Ip provided: use: 192.168.178.20\n");
		argv[1] = "192.168.178.20";
	}
	ip_addr = argv[1];
	port = 1234;
	do {
		printf("Connecting to server at %s:%d ... \n", ip_addr, port);
	} while (!tcp.setup(ip_addr, port));
	printf("Connected!");

	exit_app = false;
	std::signal(SIGTERM, signal_handler); // Register  signal interrupt handler
	bool bIsFirstCall = true;
	bool bRef = &bIsFirstCall;
	while (!exit_app) {
		//channel->update();
		TimerFunc(bRef);
		DisplayFunc();
		IdleFunc();
		if(!exit_app)usleep(100000);
	}
	motor->closeDevice(); // close EPOS2
	printf("\n -------- Delete motor object quit main!");
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