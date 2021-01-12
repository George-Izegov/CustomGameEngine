#include "AISystem.h"

AISystem::AISystem()
{
	m_AIObjectManager = std::make_shared<AIObjectManager>();
}

void AISystem::Update(float dt)
{
	for (auto agent : m_Agents)
	{
		agent->update(0.0f, dt);
	}
}

bool AISystem::AddAgent(AIAgent* agent)
{
	m_Agents.push_back(agent);

	return true;
}

void AISystem::Init()
{
	for (auto agent : m_Agents)
	{
		SPBehaviorTree bt = m_AIObjectManager->CreateBehaviorTree(agent->BTFilename, agent);

		agent->m_Tree = bt;
	}
}