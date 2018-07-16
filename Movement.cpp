#include "Movement.h"


Movement* Movement::pInstance = nullptr;
Movement::Movement()
{
}


Movement * Movement::getInstance()
{
	if (!pInstance)
	{
		pInstance = new Movement();
	}
	return pInstance;
}

Movement::~Movement()
{
}

void Movement::vClearMovementQueue()
{
	std::queue<std::shared_ptr<Toolbox::HostData>> empty;
	std::swap(movement_queue, empty);
}
