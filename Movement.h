#pragma once
#include "Toolbox.h"
class Movement
{
public:
	static Movement* pInstance;
	static Movement* getInstance();
	~Movement();
	std::queue<std::shared_ptr<Toolbox::HostData>>movement_queue;
	void vClearMovementQueue();
private:
	Movement();

};

