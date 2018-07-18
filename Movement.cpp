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
	vecMovementqueue.clear();
}

void Movement::vConcatenateSuccessiveMovements()
{
	Toolbox::HostData tempHostData;
	// Check if we have movements wich are of the same velocity and direciton followed by each other
	if (vecMovementqueue.size() < 2) // makes no sense to go on
	{
		return;
	}
	else
	{
		for (int i = 0; i < vecMovementqueue.size(); i++)
		{
			//tempHostData = movement_queue.at
			// Check for speakerID,direction and speed
			//if (movement_queue.front)
		}

	}
}
