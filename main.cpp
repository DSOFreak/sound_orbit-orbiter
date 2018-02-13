

#include "fcntl.h"
#include <chrono>
#include <lirc/config.h>
#include <lirc/lirc_client.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "wiringPi.h"
#include "cmaxonmotor.h"
#include <GL/glut.h>
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
	SUB_10
};

#define DEBOUNCE 500000000;
#define WHEELPERI float(0.029 * 3.1415)
#define RAILPERI  float(2.2 * 3.1415)
using namespace std;

unsigned short iVoltage;
int iTargetPosition, iCurrentPosition, iNumbOffs, iAngle;
unsigned char cErrorNbr, cNumb[3];
CMaxonMotor * motor;
float vol = 0.5;
FMOD::System    *fsystem;
FMOD::Channel   *channel = 0;
FMOD::DSP       *dsp;
FMOD_RESULT      result;
unsigned int     version;
void            *extradriverdata = 0;

chrono::system_clock::time_point ttNow, ttOld;
chrono::system_clock::duration ttD;

char *code, *szIRtxt;
char cTaste;
string szTxt2;
int iLircSocket;

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
// we have to use BCM numbering when initializing with wiringPiSetupSys
// when choosing a different pin number please use the BCM numbering, also
// update the Property Pages - Build Events - Remote Post-Build Event command 
// which uses gpio export for setup for wiringPiSetupSys
#define	LED	17
#define TaskTime 200

void calcAll(void) {
	iAngle = cNumb[2] + 10 * cNumb[1] + 100 * cNumb[0];
	if (iAngle > 360) iAngle = 360;
	iTargetPosition = 65536 * float(iAngle) / 360.0 * RAILPERI / WHEELPERI;
	if (iNumbOffs<2) iNumbOffs++;
	else iNumbOffs = 0;
}

void init2D(float r, float g, float b)
{
	glClearColor(r, g, b, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 600.0, 0.0, 600.0);
}
void rectangle()
{
	glBegin(GL_POLYGON);
	//glColor3f((GLfloat)0.4, (GLfloat)0, (GLfloat)0.2);
	//glVertex3f(150.0f, 200.0f, 0.0f);
	//glColor3f((GLfloat)0.4, (GLfloat)0, (GLfloat)0.8);
	//glVertex3f(450.0f, 200.0f, 0.0f);
	//glColor3f((GLfloat)0.6, (GLfloat)0, (GLfloat)0.6);
	//glVertex3f(450.0f, 400.0f, 0.0f);
	glColor3f((GLfloat)0.6, (GLfloat)0, (GLfloat)0.0);
	glVertex3f(150.0f, 400.0f, 0.0f);
	glEnd();
}
void text(char cText[], int x, int y)
{
	
	int len;
	len = strlen(cText);

	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0, 600, 0, 600);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();

	glRasterPos2i(x, y);


	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, cText[i]);
	}

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void		IdleFunc(void) {
	
	motor->GetCurrentPosition(iCurrentPosition);
	motor->ErrorNbr(&cErrorNbr);
	if (motor->ErrorCode == 0x34000007) motor->initializeDevice();
	if (cErrorNbr != 0) motor->initializeDevice();



	glutPostRedisplay();

}
void		TimerFunc(int value) {

	glutTimerFunc(TaskTime, TimerFunc, 0);	
	
	lirc_nextcode(&code);
	
	ttOld = ttNow;
	ttNow = chrono::system_clock::now();
	ttD = (ttNow - ttOld);
	// Entprellen der Infrarot Fernsteuerung
	if (ttD < (std::chrono::nanoseconds)500000000) return;

	szIRtxt = code;
	szTxt2 = code;

	if (szTxt2.find("KEY_PLAY") != -1 /*&& cTaste != PLAY*/) {
		cTaste = PLAY;
		calcAll();
		motor->Move(iTargetPosition);
		iAngle = cNumb[2] + 10 * cNumb[1] + 100 * cNumb[0];
		 for (int iii = 0; iii <= 3; iii++) cNumb[iii] = 0;			
		iNumbOffs = 0;
	}
	if (szTxt2.find("KEY_STOP") != -1) {
		cTaste = STOP;
		motor->Halt();
		for (int iii = 0; iii <= 3; iii++) cNumb[iii] = 0;
		iAngle = 0;
		iNumbOffs = 0;

	}
	if (szTxt2.find("KEY_PREVIOUS") != -1 /*&& cTaste != PREV*/) {
		cTaste = PREV;
		iTargetPosition = iTargetPosition - 16384;
	}
	if (szTxt2.find("KEY_NEXT") != -1 /*&& cTaste != NEXT*/) {
		cTaste = NEXT;
		iTargetPosition = iTargetPosition + 16384;
	}
	if (szTxt2.find("KEY_1") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_1;
		cNumb[iNumbOffs] = 1;
		calcAll();
	}
	if (szTxt2.find("KEY_2") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_2;
		cNumb[iNumbOffs] = 2;
		calcAll();
	}
	if (szTxt2.find("KEY_3") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_3;
		cNumb[iNumbOffs] = 3;
		calcAll();
	}
	if (szTxt2.find("KEY_4") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_4;
		cNumb[iNumbOffs] = 4;
		calcAll();
	}
	if (szTxt2.find("KEY_5") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_5;
		cNumb[iNumbOffs] = 5;
		calcAll();
	}
	if (szTxt2.find("KEY_6") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_6;
		cNumb[iNumbOffs] = 6;
		calcAll();
	}
	if (szTxt2.find("KEY_7") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_7;
		cNumb[iNumbOffs] = 7;
		calcAll();
	}
	if (szTxt2.find("KEY_8") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_8;
		cNumb[iNumbOffs] = 8;
		calcAll();
	}
	if (szTxt2.find("KEY_9") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_9;
		cNumb[iNumbOffs] = 9;
		calcAll();
	}
	if (szTxt2.find("KEY_0") != -1 /*&& cTaste != NEXT*/) {
		cTaste = KEY_0;
		cNumb[iNumbOffs] = 0;
		calcAll();
	}
	if (szTxt2.find(">10") != -1 /*&& cTaste != NEXT*/) {
		cTaste = SUB_10;
		iTargetPosition = iTargetPosition - 1000;
	}
	if (szTxt2.find("KEY_VOLUMEUP") != -1 /*&& cTaste != NEXT*/) {
		cTaste = SUB_10;
		result = fsystem->playDSP(dsp, 0, true, &channel);
		if (vol < 1.0f) vol += 0.1f;
		result = channel->setVolume(vol);
		result = dsp->setParameterInt(FMOD_DSP_OSCILLATOR_TYPE, 0);
		result = channel->setPaused(false);
	}
	if (szTxt2.find("KEY_VOLUMEDOWN") != -1 /*&& cTaste != NEXT*/) {
		cTaste = SUB_10;
		result = fsystem->playDSP(dsp, 0, true, &channel);
		if (vol > 0.0f) vol -= 0.1f;
		result = channel->setVolume(vol);
		result = dsp->setParameterInt(FMOD_DSP_OSCILLATOR_TYPE, 0);
		result = channel->setPaused(false);
		calcAll();
	}
}
void		DisplayFunc(void)
{

	short iCurrent;

	glClear(GL_COLOR_BUFFER_BIT);

	//rectangle();

	motor->GetSupply(iVoltage, iCurrent);
	char szText[32];
	double dVoltage = double(4.25 * double(iVoltage) / 1000);
	if (dVoltage <= 12.1)
	{
		init2D(1.0, 0.0, 0.0);
	}
	else if (dVoltage <= 13.0) init2D(0.6, 0.3, 0.0);
	else init2D(0.0, 0.6, 0.0);

	sprintf(szText, "Voltage: %2.2fV", dVoltage);
	text(szText, 200, 220);

	sprintf(szText, "Current: %d mA", iCurrent);
	text(szText, 200, 240);

	sprintf(szText, "Position: %d", iCurrentPosition);
	text(szText, 200, 260);

	sprintf(szText, "Target: %d", iTargetPosition);
	text(szText, 200, 280);


	sprintf(szText, "Error: %x", motor->ErrorCode);
	text(szText, 200, 190);

	sprintf(szText, "Error#: %x", cErrorNbr);
	text(szText, 200, 170);

	sprintf(szText, "Angle: %d", iAngle);
	text(szText, 200, 120);

	sprintf(szText, "%f %f", RAILPERI, WHEELPERI);
	text(szText, 200, 100);

	switch (cTaste) {
	case 0:break;
	case PLAY:text("Run", 200, 150); break;
	case STOP:text("Stop", 200, 150); break;
	case PREV:text("Backward", 200, 150); break;
	case NEXT:text("Forward", 200, 150); break;
	case KEY_1:text("001", 200, 150); break;
	case KEY_2:text("002", 200, 150); break;
	case KEY_3:text("003", 200, 150); break;
	case KEY_4:text("004", 200, 150); break;
	case KEY_5:text("005", 200, 150); break;
	case KEY_6:text("006", 200, 150); break;
	case KEY_7:text("007", 200, 150); break;
	case KEY_8:text("008", 200, 150); break;
	case KEY_9:text("009", 200, 150); break;
	case KEY_0:text("010", 200, 150); break;
	default: {}
	}

	//if (code != NULL) {
	//	sprintf(szText, "Test: 0x%x", *code);
	//	text(szIRtxt, 200, 150);
	//}
	//else {
	//	//sprintf(szIRtxt);
	//	text("NULL", 200, 150);
	//}

	glFlush();
}
void		ReshapeFunc(int width, int height)
{
	//** Function called when the window is created or resized
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(20, (float)width / (float)height, 5, 15);
	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}
void		KeyboardFunc(unsigned char key, int x, int y)
{
	//** Function called when a key is hit
	//int foo;

	//foo = x + y; /* Has no effect: just to avoid a warning */
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

		/**code = 10;*/
		//iTargetPosition = (aa+1)*1000;
	}
	if ('z' == key || 'Z' == key) {
		szIRtxt = "INIT";
	}

	glutPostRedisplay();
}


int main(int argc, char **argv)
{

	char InterfaceName[] = "USB0";
	motor = new CMaxonMotor(InterfaceName, 1);
	motor->initializeDevice(); // initialize EPOS2

	//int flags;

	//if ((flags = fcntl(sock_descriptor, F_GETFL, 0)) < 0)
	//{
	//	/* Handle error */
	//} 

	code = new char;
	cTaste = 0;
	//code = NULL;
	//strcpy(code, "aa");

	struct lirc_config *config;
	lirc_cmd_ctx * ctx;

	//Initiate LIRC. Exit on failure
	iLircSocket = lirc_init("irexec", 1);
	if (iLircSocket == -1)
		exit(EXIT_FAILURE);

	//lirc_command_init(ctx, "O_NONBLOCK");

	lirc_readconfig("/etc/lirc/RM-D420.conf", &config, NULL);
	///*int abc = */fcntl(iLircSocket, F_SETFL, O_NONBLOCK);
	lirc_nextcode(&code);

	//if (lirc_readconfig(configfile, &config, NULL) != 0) {
	//	fputs("Cannot parse config file\n", stderr);
	//	return EXIT_FAILURE;

	///* Creation of the window GLUT*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Moving Speaker V0.1.0");
	init2D(0.0f, 0.0f, 0.0f);

	/* Declaration of the callbacks */
	glutDisplayFunc(&DisplayFunc);
	glutReshapeFunc(&ReshapeFunc);
	glutKeyboardFunc(&KeyboardFunc);
	glutIdleFunc(&IdleFunc);
	
	TimerFunc(0);
	
	glutMainLoop();
	


	//motor->Halt();
	//motor->DisableDevice();
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