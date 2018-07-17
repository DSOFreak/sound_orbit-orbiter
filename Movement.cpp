#include "Movement.h"


std::shared_ptr<Movement> Movement::pInstance = nullptr;
Movement::Movement()
{
}


std::shared_ptr<Movement>  Movement::getInstance()
{
	if (!pInstance)
	{
		pInstance = std::make_shared<Movement>();
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
