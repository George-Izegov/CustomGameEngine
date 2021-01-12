#include "Katamari.h"

Katamari::Katamari()
{
	m_Model = 0;
}

Katamari::~Katamari()
{
}

void Katamari::Load()
{
	//fucking c++| this is base.load();
	Gameobject::Load();
}

void Katamari::Unload()
{
	Gameobject::Unload();
	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
}

HRESULT Katamari::Init(HWND hwnd, ID3D11Device* g_pd3dDevice)
{
	HRESULT result = S_OK;
	Gameobject::Init(hwnd, g_pd3dDevice);
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return E_FAIL;
	}
	// Initialize the model.
	result = m_Model->Initialize(g_pd3dDevice, "GameEngine/Data/Objects/sphere.obj", L"GameEngine/Data/Objects/earth.tga");
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize Katamari model.", L"Error", MB_OK);
		return E_FAIL;
	}
	return S_OK;
}

bool Katamari::Update()
{

	return false;
}

void Katamari::Translate(Vector3 v3)
{
	v3 *= speed;
 	m_Rotation *=  XMMatrixRotationZ(-v3.x) * XMMatrixRotationX(v3.z);

	//todo clamp dia movement
	m_Translation *= XMMatrixTranslation(v3.x, 0.0f, v3.z);
	m_Transform->trs = m_Rotation * m_Translation;
}
