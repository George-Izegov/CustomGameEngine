#include "KataVictim.h"


KataVictim::KataVictim()
{
	m_Model = 0;
	m_HasParent = false;
}

KataVictim::~KataVictim()
{
}

void KataVictim::Load()
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

void KataVictim::Unload()
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

HRESULT KataVictim::Init(HWND hwnd, ID3D11Device* g_pd3dDevice)
{
	HRESULT result = S_OK;
	Gameobject::Init(hwnd, g_pd3dDevice);
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return E_FAIL;
	}
	// Initialize the model.
	result = m_Model->Initialize(g_pd3dDevice, "GameEngine/Data/Objects/rubiks-cube.obj", L"GameEngine/Data/Objects/moonmap.tga");
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize Cube model.", L"Error", MB_OK);
		return E_FAIL;
	}
	m_Scale = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	return S_OK;
}

HRESULT KataVictim::Init(HWND hwnd, LPCWSTR model_filename, LPCWSTR texture_filename, Vector3 new_scale, ID3D11Device* g_pd3dDevice)
{
	HRESULT result = S_OK;
	Gameobject::Init(hwnd, g_pd3dDevice);
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return E_FAIL;
	}

	std::wstring mf = model_filename;
	std::string str(mf.begin(), mf.end());
	// Initialize the model.
	result = m_Model->Initialize(g_pd3dDevice, str, texture_filename);
	if (FAILED(result))
	{
		wchar_t pretext[200];
		swprintf(pretext, 200, L"Could not initialize %s", model_filename);
		MessageBox(hwnd, model_filename, L"Error", MB_OK);
		return E_FAIL;
	}
	m_Scale = XMMatrixScaling(new_scale.x, new_scale.y, new_scale.z);
	//m_Scale = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	return S_OK;
}

bool KataVictim::Update()
{
	return false;
}

bool KataVictim::Update(Katamari* k)
{

	m_Transform->trs = m_Scale * m_Translation * k->m_Rotation * k->m_Translation;
	return true;
}

void KataVictim::AttachTo(Gameobject* go)
{
	m_HasParent = true;
	m_ParentTRS = go->m_Transform->trs.Invert();

	Vector3 t = Vector3(m_Translation._41, m_Translation._42, m_Translation._43);
	Vector3 s = Vector3(m_Scale._11, m_Scale._22, m_Scale._33);
	
	m_Translation = XMMatrixTranslation((t.x*s.x) / 12, (t.y*s.y) / 12, (t.z*s.z) / 12);
}

void KataVictim::Place(Vector3 pos)
{
	m_Translation.Translation(pos);
	m_Transform->trs = m_Scale * m_Translation;
}



