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

HRESULT AIAgent::Init(HWND hwnd, LPCWSTR model_filename, LPCWSTR texture_filename, Vector3 new_scale, ID3D11Device* d3ddevice)
{
	bool result;
	Gameobject::Init(hwnd, d3ddevice);
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return E_FAIL;
	}
	std::wstring mf = model_filename;
	std::string str(mf.begin(), mf.end());
	// Initialize the model.
	result = m_Model->Initialize(d3ddevice, str, texture_filename);
	if (FAILED(result))
	{
		wchar_t pretext[200];
		swprintf(pretext, 200, L"Could not initialize %s", model_filename);
		MessageBox(hwnd, model_filename, L"Error", MB_OK);
		return E_FAIL;
	}
	m_Scale = XMMatrixScaling(new_scale.x, new_scale.y, new_scale.z);

	return result;
}