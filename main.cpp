

#include "fcntl.h"
#include <chrono>
#include <queue>
#include <mutex>
#include <csignal>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <sstream>
#include <string.h>
#include "wiringPi.h"
#include "cmaxonmotor.h"
#include "fmod.hpp"
#include "TCPClient.h"
#include "Toolbox.h"
#include "StimuliLibrary.h" 
#include "Movement.h"
#include "DataToCSV.h"
#include "tcpParameterRequestHandler.h"
#include "udpServr.h"
using namespace std;



unsigned short iVoltage;
int iCurrentPosition, iNumbOffs;
unsigned char cErrorNbr, cNumb[3]; // Motor errors
CMaxonMotor*  pMotor;
std::shared_ptr<tcpParameterRequestHandler> pTCPParameterRequestHandler;
std::shared_ptr<Movement> pMovement;
bool exit_app;
std::queue<std::string> tcp_queue;
std::mutex tcp_mutex;
std::mutex movementTimerMutex;
TCPClient tcp;
char *ip_addr;
int port;
std::shared_ptr<StimuliLibrary> pStimuliLib;

//long long llDebugNumOfTCPFuncCalls = 0;

void tcp_func() {
	std::string msg;

	while (!exit_app) {
		//llDebugNumOfTCPFuncCalls++;
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
		usleep(10000);
	}
}

void udp_func() {
	std::string msg;
	udpServr* pUdpServer = udpServr::getInstance();
	while (!exit_app) {
		//llDebugNumOfTCPFuncCalls++;
		//cout << "inside udp func..... " << msg << endl;
		msg = pUdpServer->vRecvUDP();
		if (msg.length() == 0) {
			std::cout << "Reconnecting..." << std::endl;
			pUdpServer->vExit();
			pUdpServer->vSetup();
			sleep(2);
		}
		else if (msg.compare(IGNORE_MESSAGE_INDICATOR) != 0) // is it a safety repeat message ?
		{
			// non safety repeat message -> VALID message .. up to this point
			pUdpServer->mutexUDPQueue.lock();
			//cout << "pushin..... " << msg << endl;
			pUdpServer->udp_queue.push(msg);
			pUdpServer->mutexUDPQueue.unlock();
			usleep(10000);
		}
	}
	cout << "LEAVING UDP FUNC" << endl;
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





void IdleFunc(void) {
	pMotor->getCurrentPosition(iCurrentPosition);
	pMotor->ErrorNbr(&cErrorNbr);
	//if (pMotor->ErrorCode == 0x34000007) pMotor->initializeDevice();
	//if (cErrorNbr != 0) pMotor->initializeDevice();
}

bool movement_skip;
bool stimuli_skip;
void vProcessMovement()
{
	float fAngleDegree;
	std::cout << "vProcessMovement:" << std::endl;
	shared_ptr<Toolbox::HostData> hostData;
	if (!pMovement->vecMovementqueue.empty())
	{
		hostData = pMovement->vecMovementqueue.front();
		pMovement->vecMovementqueue.erase(pMovement->vecMovementqueue.begin());
	}
	else
	{
		return;
	}


	if (hostData->direction == 1) { // Dir 1 = clockwise
		fAngleDegree = hostData->fAngularDistance * -1.0; // Correct
	}
	if (hostData->direction == 2) { // Dir 2 = counterclockwise
		fAngleDegree = hostData->fAngularDistance;
	}
	

	if (hostData->direction != 0) { // Dir 0 = no movement
		long lMotorDataTargetPosition = pMotor->lConvertAngleInDegreeToMotorData(fAngleDegree);
		pMotor->setCurrentTargetPositionInMotorData(lMotorDataTargetPosition);
		pMotor->setSpeed(hostData->speed, fAngleDegree);
		pMotor->Move(pMotor->lgetCurrentTargetPositionInMotorData());
		pMotor->currentlyProcessedMovementData = hostData;
	}
}
//long long llNumberOfRelevantThreadCalls = 0;
void TimerFunc() {
	udpServr* pUdpServer = udpServr::getInstance();
	//long long debug = 0;
	while (true) //es ist entweder gerade bewegung oder die queue ist leer
	{
		//cout << "INSIDE TIMER FUNC" << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//llNumberOfRelevantThreadCalls++;
	//if ((pMotor->lgetCurrentTargetPositionInMotorData() != NO_MOVEMENT_IN_PROCESS) || (pMovement->vecMovementqueue.empty()))
		std::string host_data_raw;
		std::string host_data_raw_Copy;
		while(!pUdpServer->udp_queue.empty()) {

			pUdpServer->mutexUDPQueue.lock();
			host_data_raw = pUdpServer->udp_queue.front(); // Get tcp messages
			pUdpServer->udp_queue.pop();
			pUdpServer->mutexUDPQueue.unlock();
			std::cout << "\n Raw hostData input: " << host_data_raw << std::endl;
			//std::this_thread::sleep_for(std::chrono::milliseconds(20)); // das istn ur anstelle von 			((pMotor->lgetCurrentTargetPositionInMotorData() != NO_MOVEMENT_IN_PROCESS) || (pMovement->vecMovementqueue.empty())) .. damit es etwas feinierter ist
			/* PROTOCOL INTERPRETATION */
			int speakerIDX;
			if (host_data_raw.length() > 0)
			{ // If a tcp-message has arrived
				host_data_raw_Copy = host_data_raw;
				movementTimerMutex.lock();
				if (host_data_raw_Copy.find("G_P_A") == std::string::npos)//debug avoid the printfs
				{
				//	std::cout << "\n Raw hostData input: " << host_data_raw_Copy << std::endl;
				}


				// FIRST: Check if it is a get or set request for raspi data
				char charIsGetOrSetRequest = host_data_raw_Copy.at(0);
				std::cout << "DEBUG charIsGetOrSetRequest " << charIsGetOrSetRequest << std::endl;
				if (charIsGetOrSetRequest == 'G')
				{
					std::string strsAnsnwerToServerRequest;
					strsAnsnwerToServerRequest = pTCPParameterRequestHandler->interpretRequest(host_data_raw_Copy);

					tcp.Send(strsAnsnwerToServerRequest);
					//std::cout << "Battery Voltage" << strsAnsnwerToServerRequest << endl;
					std::cout << "DEBUG debug nr: "<< endl;
				}
				else if (charIsGetOrSetRequest == 'S') // no answer sned needed
				{
					std::string strsAnsnwerToServerRequest;
					strsAnsnwerToServerRequest = pTCPParameterRequestHandler->interpretRequest(host_data_raw_Copy);
				}
				else
				{
					shared_ptr<Toolbox::HostData> hostData(new Toolbox::HostData(Toolbox::decodeHostData(host_data_raw_Copy))); // decode host data
					if (hostData->mov_queued) { // Add new data to queue
						std::cout << "Add new data to queue" << endl;

						if (hostData->fAngularDistance > 0.0)
						{
							pMovement->vecMovementqueue.push_back(hostData);
						}
						movement_skip = false;
					}
					else { // Clear Queue and Add new Data to Queue
						std::cout << " Clear Queue and Add new Data to Queuee" << endl;
						movement_skip = true;
						while (!pMovement->vecMovementqueue.empty())
						{
							std::cout << " try to delete queue begin" << endl;
							pMovement->vecMovementqueue.erase(pMovement->vecMovementqueue.begin());
							std::cout << " delete queue begin OK" << endl;
						}
						std::cout << " try to vecMovementqueue pushback begin" << endl;
						pMovement->vecMovementqueue.push_back(hostData);
						std::cout << " vecMovementqueue pushback begin OK" << endl;
					}
					
					if (hostData->stim_queued) { // Add new data to queue
					std::cout << "1" << endl;
					pStimuliLib->mutexStimuli.lock();
					if (hostData->toBeTriggerd == 1) // only if it i a stimulus which also should be played (and not a dummy placeholder protocol values)
					{
					pStimuliLib->stimuli_queue.push(hostData);
					}
					stimuli_skip = false;
					std::cout << "2" << endl;
					pStimuliLib->mutexStimuli.unlock();
					}
					else { // Clear Queue and Add new Data to Queue
					pStimuliLib->mutexStimuli.lock();
					std::cout << "3" << endl;
					stimuli_skip = true;
					std::cout << "4" << endl;
					while (!pStimuliLib->stimuli_queue.empty())
					{
						std::cout << "5" << endl;
						pStimuliLib->stimuli_queue.pop();
					}
					std::cout << "6" << endl;
					pStimuliLib->stimuli_queue.push(hostData);
					pStimuliLib->mutexStimuli.unlock();
					}
					std::cout << "Add new data to queue DONE" << endl;
					
				}
				movementTimerMutex.unlock();
			}
	}

	}
}
void vMovementThread(bool &bIsFirstCall)
{
	// falls nix geht... mache stimulilib shared pntr !!!!!!!!!!!!
	std::thread movementThread{ [&]()
	{
		//long long llNumberOfRelevantThreadCalls = 0;
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			//llNumberOfRelevantThreadCalls++;
			//cout << "** MOVMENT THREAD START" << endl;
		// MOVEMENT PROCESSING 
			movementTimerMutex.lock();
			cout << "trying to call bTryToAddMovementDataToCurrentMovement"<<endl;
			if (!pMotor->bTryToAddMovementDataToCurrentMovement(pMovement))
			{

				if (!pMovement->vecMovementqueue.empty())// movement pending 
				{
					cout << "inside vMovementThread" << endl;
					if (pMotor->reachedTarget(1337, 1337, 1337) || movement_skip) // (movementFinnished OR Skip_this_movement)
					{
						cout << "DEBUG reaced target 123" << endl;
						if (movement_skip)
						{
							cout << "skip the movement" << endl;
						}
						cout << "DEBUG reaced target 1234" << endl;
						vProcessMovement();
						cout << "DEBUG reaced target 12345" << endl;
					}
					// Here we ware if we have some movements in our queue which we want to do but still other movements are going on
					else if ((bIsFirstCall == true) && (pMotor->lgetCurrentTargetPositionInMotorData() == NO_MOVEMENT_IN_PROCESS))
					{
						bIsFirstCall = false;
						cout << endl << endl << endl << endl << "´++++++++++++++++++++++++++++++++++++++++++++++++++++++++Movement due to first call" << endl << endl;
						vProcessMovement();
						// is Motor moving? else: process further movement
						//cout << "++++++++++++++++++++++++++Motor not in position we wait for movement to finish" << endl;
						//vProcessMovement();
					}
				}
			}
			movementTimerMutex.unlock();
		}
		}}; movementThread.detach();
}

/* STIMULI PROCESSING */
void vStimuliThread()
{
	// falls nix geht... mache stimulilib shared pntr !!!!!!!!!!!!
	std::thread stimuliThread{ [&]()
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			pStimuliLib->mutexStimuli.lock();
			if ((pStimuliLib->pInstance != nullptr))
			{
				std::cout << "pStimuliLib->updateFSystem();" << endl;
				pStimuliLib->updateFSystem();
				// Check if there is a protocol hicjacking
				std::cout << "Check if there is a protocol hicjacking" << endl;
				if (!pStimuliLib->bAdaptStimulusParametersDueToHijacking(pMovement->vecMovementqueue)) // not protocl adaption, process as usual
				{
					//std::cout << "Check if there is a protocol hicjacking DONE" << endl;
					//cout << "No Hijacking" << endl;
					if (pStimuliLib->bGetIsThereAFractionLeftToPlay())
					{
						printf("\n We have a fraction left to play of %d milliseconds\n");
						pStimuliLib->playStimuli(); // Enter here only if (audioFileLength_ms < desiredDuration_ms)
					}
					else if (!pStimuliLib->stimuli_queue.empty())
					{
						cout << "We try to play a stimulus if it has to be triggered" << endl;
						pStimuliLib->vPlayStimulusIfToBeTriggered();
					}
				}
				//std::cout << "Check if there is a protocol hicjackingDONE" << endl;
				if (pStimuliLib->bGetResetStimuliLib())
				{
					pStimuliLib->vClearStimuliQueue();
					//pStimuliLib->vDoRebootOfStimuliLib();
					//pStimuliLib = StimuliLibrary::getInstance();
					//printf("Reboot DONE \n");
				}
				pStimuliLib->mutexStimuli.unlock();
			}

		}
	} };
	stimuliThread.detach(); // Prevents the thread from bein destroyed when the its out of scope
}


int main(int argc, char **argv)
{
	RaspiConfig rRaspiConfig = RaspiConfig();
	pStimuliLib = StimuliLibrary::getInstance();
	printf("Starting Orbiter Program.");
	pMovement = Movement::getInstance();
	char InterfaceName[] = "USB0";
	pMotor = CMaxonMotor::getInstance();
	pMotor->initializeDeviceNew(); // initialize EPOS2

	pTCPParameterRequestHandler = std::make_shared<tcpParameterRequestHandler>();

	
	
	if (argc == 1)
	{
		printf("No Ip provided: use: 192.168.178.23\n");
		argv[1] = "192.168.178.23";
	}
	ip_addr = argv[1];
	port = 1234;
	/*
	// TCP CONNECTION
	std::thread tcpThread(tcp_func);
	
	do {
		printf("Connecting to server at %s:%d ... \n", ip_addr, port);
		usleep(1000000);
	} while (!tcp.bGetbIsConnectionEstablished());
	printf("Connected!");*/
	
	// UDP CONNECTION
	exit_app = false;
	std::thread udpThread(udp_func);
	std::thread timerThread(TimerFunc);
	/*udp_client_server::udp_server updClient = udp_client_server::udp_server(ip_addr, port);
	char* msg;
	int iMsgLength = 0;
	while (iMsgLength == 0)
	{
		printf("Waiting for UDP Broadcast data \n");
		iMsgLength = updClient.recv(msg, 100);
		usleep(1000000);
		printf("Data: %s\n", msg);
	}*/
	


	// Start the application

	bool bIsFirstCall = true;
	bool bRef = &bIsFirstCall;
	vStimuliThread();
	vMovementThread(bRef);
	while (!exit_app)
	{
		//DEBUG
		//cout << "CALL FROM MAIN" << endl;
		//pMotor->getCurrentPosition(iCurrentPosition); 

		usleep(1000000);
		/*if (pMotor->bClearFaultIfInFaultState()) die idee ist gut die fehler zu cleareen, aber so verursachen wir unendlich viele neustarts
		{
			printf("\n\n\n ----------------------- tried to clear a fault state -> restatrt the motor ------------ \n\n\n");
			// tried to clear a fault state -> restatrt the motor
			pMotor = std::make_shared<CMaxonMotor>();
			//pMotor->initializeDevice(); // initialize EPOS2
			pMotor->initializeDeviceNew(); // initialize EPOS2
		}*/
	}
	printf("\n Delete motor object quit main!");
	pMotor->closeDevice(); // close EPOS2
	return 0;
}