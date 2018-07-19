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
		//std::cout << "return vConcatenateSuccessiveMovements" << std::endl;
		return;
	}
	std::cout << " Going in " << std::endl;
	std::shared_ptr<Toolbox::HostData> pTempHostDataStart;
	std::shared_ptr<Toolbox::HostData> pTempHostDataNext;
	// first we remove all alues which are not for us from the queue
	vecMovementqueue.erase(std::remove_if(vecMovementqueue.begin(), vecMovementqueue.end(), bIsNotOwnSpeakerIndex), vecMovementqueue.end());
	// Check if we have movements wich are of the same velocity and direciton followed by each other
	if ((vecMovementqueue.size() < 2) || (vecMovementqueue.empty())) // makes no sense to go on
	{
		//std::cout << "return vConcatenateSuccessiveMovements" << std::endl;
		return;
	}
	else
	{
		std::vector<int> veciConsecutiveMovementsIndices;
		//We only go in if we find successive movements 
		for (int i = 0; i < vecMovementqueue.size()-1; i++)
		{
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
		//Delte duplicates
		veciConsecutiveMovementsIndices.erase(std::unique(veciConsecutiveMovementsIndices.begin(), veciConsecutiveMovementsIndices.end()), veciConsecutiveMovementsIndices.end());
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
				vecTempMovementQueue.push_back(vecMovementqueue.at(veciConsecutiveMovementsIndices.at(k)));
			}

			if ((veciConsecutiveMovementsIndices.at(k)+1) == (veciConsecutiveMovementsIndices.at(k+1)))//successive?
			{
				fAngularDistance += vecMovementqueue.at(veciConsecutiveMovementsIndices.at(k + 1))->angularDistance;
				bIsSuccessiveMovement = true;
			}
			else
			{
				// Store the accumulated data
				vecTempMovementQueue.back()->angularDistance = fAngularDistance;
				std::cout << "ecTempMovementQueue.back()->angularDistanc.push_back GOOD" << std::endl;
				fAngularDistance = 0;
				//Not a consecutive movement
				bIsSuccessiveMovement = false;
			}	
			// if we are at the end of the list .. store!
			if (bIsSuccessiveMovement && ((k + 2) == veciConsecutiveMovementsIndices.size()))
			{
				// Store the accumulated data
				vecTempMovementQueue.back()->angularDistance = fAngularDistance;
				std::cout << "if we are at the end of the list .. store!" << std::endl;
				fAngularDistance = 0;
				//Not a consecutive movement
				bIsSuccessiveMovement = false;
			}
		}
		// we copy our vector
		vecMovementqueue = vecTempMovementQueue;
	}
}
