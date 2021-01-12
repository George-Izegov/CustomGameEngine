#pragma once

#include "BrainTree.h"

const std::wstring DataPath = L"Data/AIData/";
class AIObjectManager
{
private:

	SPBlackboard CreateBlackboard(std::wstring FileName);

public:

	SPBehaviorTree CreateBehaviorTree(std::wstring FileName, AIAgent* owner);
};

