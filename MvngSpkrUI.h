#ifndef _CMvngSpkrUI_
#define _CMvngSpkrUI_





/*************************************************************************************************************************************
**                  ARTORG Center University of Bern
**************************************************************************************************************************************
**
** FILE:            MvngSpkrUI.h
**
** Summary:         Exported Functions for Linux 32-Bit User Interface programed with <curses.h>
**					Class for the managment of the User Interface
**
** Date:            05.12.2017 wyt
** Dev. Platform:   Visual Studio 2015
** Target:          Linux (Ubuntu Raspbian)
** Written by:      ARTORG, CH-3008 Bern
**
** Changes:        0.1.0.0    (05.12.2017): Initial Version
** 			       0.1.0.1    (05.12.2017): Initial Version
*************************************************************************************************************************************/



class CMvngSpkrUI
{
public:
	CMvngSpkrUI();
	~CMvngSpkrUI();
	void InitMenu(void);
	void PositionMenu(void);
	void CurrentMenu(void);
	void VelocityMenu(void);
	void ErrorScreen(void);
	void VoltCurrShow(double Voltage, int Current);
};

#endif // !_CMvngSpkrUI_