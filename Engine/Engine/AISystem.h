#pragma once

#include <vector>
#include "AIAgent.h"
#include "AISystem/BrainTree.h"
#include "AISystem/AIObjectManager.h"

class AISystem
{
public:

	AISystem();

	void Update(float dt);

	bool AddAgent(AIAgent* agent);

	void Init();

private:
	std::vector<AIAgent*> m_Agents;

	std::shared_ptr<AIObjectManager> m_AIObjectManager;
};