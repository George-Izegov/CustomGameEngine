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

bool Gameplane::Init(HWND hwnd, D3DClass* d3d)
{
	bool result;
	Gameobject::Init(hwnd, d3d);
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return false;
	}
	// Initialize the model.
	result = m_Model->Initialize(Gameobject::m_D3D->GetDevice(), "Data\\Objects\\m_plane.obj", L"../Engine/brick.tga");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize model.", L"Error", MB_OK);
		return false;
	}
	m_Scale = XMMatrixScaling(3.75f, 3.75f, 3.75f);
	m_Translation = XMMatrixTranslation(0.0f, -1.0f, 0.0f);
	m_Transform->trs = m_Scale * m_Translation;
	return result;
}

bool Gameplane::Update()
{
	return false;
}
