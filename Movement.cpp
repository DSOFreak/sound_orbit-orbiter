#include "Movement.h"
#include "RaspiConfig.h"
#include <algorithm>
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


inline bool bIsNotOwnSpeakerIndex(std::shared_ptr<Toolbox::HostData> pTempHostData)
{
	if (pTempHostData->speakerIDX != RaspiConfig::ownIndex)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void Movement::vConcatenateSuccessiveMovements()
{
	if ((vecMovementqueue.size() < 2) || (vecMovementqueue.empty()))
	{
		return;
	}
	std::cout << " Going in " << std::endl;
	std::shared_ptr<Toolbox::HostData> pTempHostDataStart;
	std::shared_ptr<Toolbox::HostData> pTempHostDataNext;
	// first we remove all alues which are not for us from the queue
	vecMovementqueue.erase(std::remove_if(vecMovementqueue.begin(), vecMovementqueue.end(), bIsNotOwnSpeakerIndex), vecMovementqueue.end());
	std::cout << " vecMovementqueue.erase(std::remove_if(vecMovementqueue.begin(), " << std::endl;
	// Check if we have movements wich are of the same velocity and direciton followed by each other
	if ((vecMovementqueue.size() < 2) || (vecMovementqueue.empty())) // makes no sense to go on
	{
		std::cout << "return" << std::endl;
		return;
	}
	else
	{
		std::vector<int> veciConsecutiveMovementsIndices;
		//We only go in if we find successive movements 
		std::cout << "We only go in if we find successive movements " << std::endl;
		for (int i = 0; i < vecMovementqueue.size()-1; i++)
		{
			std::cout << "ecMovementqueue.at(i);" << std::endl;
			pTempHostDataStart = vecMovementqueue.at(i);
			std::cout << " pTempHostDataStart = vecMovementqueue.at(i);" << std::endl;
			pTempHostDataNext = vecMovementqueue.at(i+1);
			std::cout << " pTempHostDataStart = vecMovementqueue.at(i+1);" << std::endl;
			if ((pTempHostDataStart->direction == pTempHostDataNext->direction) && (pTempHostDataStart->speed == pTempHostDataNext->speed))
			{
				// We store the indices of the movements which can be concatenated
				veciConsecutiveMovementsIndices.push_back(i);
				veciConsecutiveMovementsIndices.push_back(i+1);
			}
		}
		std::cout << "//Delte duplicates" << std::endl;
		//Delte duplicates
		veciConsecutiveMovementsIndices.erase(std::unique(veciConsecutiveMovementsIndices.begin(), veciConsecutiveMovementsIndices.end()), veciConsecutiveMovementsIndices.end());
		std::cout << "//Delte duplicates good" << std::endl;
		if ((veciConsecutiveMovementsIndices.size() < 2) || (veciConsecutiveMovementsIndices.empty())) // makes no sense to go on
		{
			return;
		}
		// now we create a new movement vector
		std::vector<std::shared_ptr<Toolbox::HostData>> vecTempMovementQueue;

		// We concatenate the indices
		float fAngularDistance = 0;
		bool bIsSuccessiveMovement = false;
		for(int k = 0; k < (veciConsecutiveMovementsIndices.size()-1); k++)
		{
			
			if (!bIsSuccessiveMovement) // Init of new movement queue vector
			{
				std::cout << "vecTempMovementQueue.push_back" << std::endl;
				vecTempMovementQueue.push_back(vecMovementqueue.at(veciConsecutiveMovementsIndices.at(k)));
				std::cout << "vecTempMovementQueue.push_back GOOD" << std::endl;
			}

			if ((veciConsecutiveMovementsIndices.at(k)+1) == (veciConsecutiveMovementsIndices.at(k+1)))//successive?
			{
				fAngularDistance += vecMovementqueue.at(veciConsecutiveMovementsIndices.at(k + 1))->angularDistance;
				bIsSuccessiveMovement = true;
			}
			else
			{
				// Store the accumulated data
				std::cout << "ecTempMovementQueue.back()->angularDistanc.push_back GOOD" << std::endl;
				vecTempMovementQueue.back()->angularDistance = fAngularDistance;
				std::cout << "ecTempMovementQueue.back()->angularDistanc.push_back GOOD" << std::endl;
				fAngularDistance = 0;
				//Not a consecutive movement
				bIsSuccessiveMovement = false;
			}		
		}
		// we copy our vector
		std::cout << "// we copy our vectorD" << std::endl;
		vecMovementqueue = vecTempMovementQueue;
		std::cout << "// we copy our vectorDGOOD" << std::endl;
	}
}
