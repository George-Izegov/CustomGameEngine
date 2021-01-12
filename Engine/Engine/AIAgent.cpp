#include "AIAgent.h"
#include "math_statics.h"

AIAgent::AIAgent()
{
	setMaxSpeed(30.0f);
	setMass(10.0f);
	setMaxForce(500.0f);

	m_Model = 0;
}

AIAgent::AIAgent(CameraClass* camera)
{
	m_Camera = camera;

	setMaxSpeed(10.0f);
	setMass(10.0f);
	setMaxForce(100.0f);

	m_Model = 0;
}

void AIAgent::update(const float currentTime, const float elapsedTime)
{
	if (m_Tree)
		m_Tree->tick();

	m_Transform->trs.Translation(Vec3ToVector3(position()));
}

bool AIAgent::Init(HWND hwnd, std::string model_filename, LPCWSTR texture_filename, Vector3 new_scale, D3DClass* d3d)
{
	bool result;
	Gameobject::Init(hwnd, d3d);
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return false;
	}
	// Initialize the model.
	result = m_Model->Initialize(Gameobject::m_D3D->GetDevice(), model_filename, texture_filename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize model.", L"Error", MB_OK);
		return false;
	}
	m_Scale = XMMatrixScaling(new_scale.x, new_scale.y, new_scale.z);

	return result;
}