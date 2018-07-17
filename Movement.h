#pragma once
#include "Toolbox.h"
#include <memory>
class Movement
{
public:
	static std::shared_ptr<Movement> pInstance;
	static std::shared_ptr<Movement> getInstance();
	~Movement();
	std::queue<std::shared_ptr<Toolbox::HostData>>movement_queue;
	void vClearMovementQueue();
	Movement();
private:


};

