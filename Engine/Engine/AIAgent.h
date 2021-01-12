#pragma once

#include "Gameobject.h"
#include "AISystem/SimpleVehicle.h"
#include "AISystem/BrainTree.h"
#include "cameraclass.h"

class AIAgent : public Gameobject, public OpenSteer::SimpleVehicle
{
public:
	friend class AISystem;

	AIAgent();

	AIAgent(CameraClass* camera); 

	void update(const float currentTime, const float elapsedTime) override;

	bool Init(HWND, std::string model_filename, LPCWSTR texture_filename, Vector3, D3DClass*) override;

	void SetBTFilename(std::wstring name) { BTFilename = name; }

public:
	ModelClass* m_Model;

	// player
	CameraClass* m_Camera;
private:

	
	SPBehaviorTree m_Tree;

	std::wstring BTFilename;

};