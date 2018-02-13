#include "MvngSpkrUI.h"
#include <curses.h>

#define _RED_		1
#define _GREEN_		2
#define _NORMAL_	3
#define _BLUE_		4



CMvngSpkrUI::CMvngSpkrUI()
{
	initscr();					/* Start curses mode 		*/
	raw();						/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();					/* Don't echo() while we do getch */

	start_color();

	init_pair(_RED_, COLOR_RED, COLOR_BLACK);
	init_pair(_GREEN_, COLOR_GREEN, COLOR_BLACK);
	init_pair(_NORMAL_, COLOR_WHITE, COLOR_BLACK);
	init_pair(_BLUE_, COLOR_BLUE, COLOR_BLACK);

}
CMvngSpkrUI::~CMvngSpkrUI()
{
	move(20, 2);
	printw("************ ENDE *******************");
	refresh();			/* Print it on to the real screen */
	endwin();			/* End curses mode		  */
}

void CMvngSpkrUI::InitMenu(void)
{

	attron(COLOR_PAIR(_NORMAL_));
	move(1, 2);
	printw("********** Moving Speaker V0.1 **********");
	move(2, 2);
	printw("*                                       *");
	move(3, 2);
	printw("*  v = velocity mode                    *");
	move(4, 2);
	printw("*  p = position mode                    *");
	move(5, 2);
	printw("*  q = quit                             *");
	move(20, 2);
	printw("****************************************");

	refresh();

}
void CMvngSpkrUI::PositionMenu(void)
{

	attron(COLOR_PAIR(_NORMAL_));
	move(1, 2);
	printw("********** Moving Speaker V0.1 **********");
	move(2, 2);
	printw("*                                       *");
	move(3, 2);
	printw("*  v = current mode                    *");
	move(5, 2);
	printw("*  v = velocity mode                    *");
	move(7, 2);
	printw("*  q = quit                             *");
	move(20, 2);
	printw("****************************************");

	attron(COLOR_PAIR(_GREEN_));
	move(4, 2);
	printw("*  p = position mode                    *");

	refresh();

}
void CMvngSpkrUI::CurrentMenu(void)
{

	attron(COLOR_PAIR(_NORMAL_));
	move(1, 2);
	printw("********** Moving Speaker V0.1 **********");
	move(2, 2);
	printw("*                                       *");
	move(4, 2);
	printw("*  p = position mode                    *");
	move(5, 2);
	printw("*  v = velocity mode                    *");
	move(7, 2);
	printw("*  q = quit                             *");
	move(20, 2);
	printw("*****************************************");

	attron(COLOR_PAIR(_GREEN_));
	move(3, 2);
	printw("*  v = current mode                    *");

	refresh();

}
void CMvngSpkrUI::VelocityMenu(void)
{

	attron(COLOR_PAIR(_NORMAL_));
	move(1, 2);
	printw("********** Moving Speaker V0.1 **********");
	move(2, 2);
	printw("*                                       *");
	move(3, 2);
	printw("*  v = current mode                    *");
	move(4, 2);
	printw("*  p = position mode                    *");
	move(7, 2);
	printw("*  q = quit                             *");
	move(20, 2);
	printw("*****************************************");

	attron(COLOR_PAIR(_GREEN_));
	move(5, 2);
	printw("*  v = velocity mode                    *");

	refresh();

}
void CMvngSpkrUI::ErrorScreen(void)
{

	attron(COLOR_PAIR(_RED_));
	move(1, 2);
	printw("********** Moving Speaker V0.1 **********");
	move(2, 2);
	printw("*     E R R O R                         *");
	move(3, 2);
	printw("*  v = current mode                     *");
	move(4, 2);
	printw("*  p = position mode                    *");
	move(5, 2);
	printw("*  v = velocity mode                    *");
	move(7, 2);
	printw("*  q = quit                             *");
	move(20, 2);
	printw("*****************************************");

	attron(COLOR_PAIR(_GREEN_));

	refresh();

}
void CMvngSpkrUI::VoltCurrShow(double dVoltage , int iCurrent)
{

	attron(COLOR_PAIR(_RED_));
	move(15, 10);

	printw("I= %d mA \tU= %2.2f V", iCurrent, dVoltage);
	//unsigned char ch = getch();
	refresh();

}
