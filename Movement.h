#pragma once
#include "Toolbox.h"
#include <memory>
class Movement
{
public:
	static std::shared_ptr<Movement> pInstance;
	static std::shared_ptr<Movement> getInstance();
	~Movement();
	std::vector<std::shared_ptr<Toolbox::HostData>>vecMovementqueue;
	void vClearMovementQueue();
	void vConcatenateSuccessiveMovements();// If we have concatenated movements with the same velocity and same direction -> add the angular distances to avoid "stops" during a trajectory of the same direcion
	Movement();
private:


};

