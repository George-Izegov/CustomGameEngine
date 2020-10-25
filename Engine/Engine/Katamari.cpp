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

bool Katamari::Init(HWND hwnd, D3DClass* d3d)
{
	bool result;
	Gameobject::Init(hwnd, d3d);
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return false;
	}
	// Initialize the model.
	result = m_Model->Initialize(Gameobject::m_D3D->GetDevice(), "Data\\Objects\\sphere.obj", L"../Engine/earth.tga");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize model.", L"Error", MB_OK);
		return false;
	}
	return result;
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
