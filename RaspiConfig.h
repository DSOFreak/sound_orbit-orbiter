#pragma once
class RaspiConfig
{
public:
	RaspiConfig();
	~RaspiConfig();
	static const int ownIndex; // This is the UNIQUE Number which identifies each raspi
	static float fWheelPerimeterRaspi; // the wheels have not the EXACT same perimeter....
};

