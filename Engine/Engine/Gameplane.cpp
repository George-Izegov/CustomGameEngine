#include "Gameplane.h"

Gameplane::Gameplane()
{
	m_Model = 0;
}

Gameplane::~Gameplane()
{
}

void Gameplane::Load()
{
	Gameobject::Load();
	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
}

void Gameplane::Unload()
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

HRESULT Gameplane::Init(HWND hwnd, ID3D11Device* g_pd3dDevice)
{
	HRESULT result = S_OK;
	Gameobject::Init(hwnd, g_pd3dDevice);
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return E_FAIL;
	}
	// Initialize the model.
	result = m_Model->Initialize(g_pd3dDevice, "Data/Objects/plane.obj", L"Data/Objects/brick.tga");
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize Plane.", L"Error", MB_OK);
		return E_FAIL;

	}
	m_Scale = XMMatrixScaling(3.75f, 3.75f, 3.75f);
	m_Translation = XMMatrixTranslation(0.0f, -1.0f, 0.0f);
	m_Transform->trs = m_Scale * m_Translation;

	return S_OK;
}

bool Gameplane::Update()
{
	return false;
}
