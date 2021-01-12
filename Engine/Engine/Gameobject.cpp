#include "Gameobject.h"

Gameobject::Gameobject()
{
	device = NULL;
	m_Transform = 0;
}

Gameobject::~Gameobject()
{
}

void Gameobject::Load()
{
}

void Gameobject::Unload()
{

}
//hate c++
bool Gameobject::IsCloser(Gameobject* other, float dist)
{
	Vector3 position;
	Quaternion rotation; //not needed but c++
	Vector3 scale;//not needed but c++
	m_Transform->trs.Decompose(scale, rotation, position);
	Vector3 otherPosition;
	Quaternion otherRotation;//not needed but c++
	Vector3 otherScale;//not needed but c++
	other->m_Transform->trs.Decompose(otherScale, otherRotation, otherPosition);
	
	float diff_x = position.x - otherPosition.x;
	float diff_y = position.y - otherPosition.y;
	float diff_z = position.z - otherPosition.z;
	
	return ((float)sqrt(diff_x * diff_x + diff_y * diff_y + diff_z * diff_z)< dist);
	return true;
}

Vector3 Gameobject::Distance(Gameobject* other)
{
	Vector3 position;
	Quaternion rotation; //not needed but c++
	Vector3 scale;//not needed but c++
	m_Transform->trs.Decompose(scale, rotation, position);
	Vector3 otherPosition;
	Quaternion otherRotation;//not needed but c++
	Vector3 otherScale;//not needed but c++
	other->m_Transform->trs.Decompose(otherScale, otherRotation, otherPosition);

	float diff_x = position.x - otherPosition.x;
	float diff_y = position.y - otherPosition.y;
	float diff_z = position.z - otherPosition.z;
	return Vector3(diff_x, diff_y, diff_z);
}

HRESULT Gameobject::Init(HWND hwnd, ID3D11Device* g_pd3dDevice)
{
	device = g_pd3dDevice;

	m_Transform = new Transform;
	m_Transform->trs = XMMatrixIdentity();
	// Create the Model.
	return S_OK;
}

HRESULT Gameobject::Init(HWND hwnd, LPCWSTR model_filename, LPCWSTR texture_filename, Vector3 new_scale, ID3D11Device* g_pd3dDevice)
{
	device = g_pd3dDevice;

	m_Transform = new Transform;
	m_Transform->trs = XMMatrixIdentity();
	// Create the Model.

	return S_OK;
}

bool Gameobject::Update()
{
	return true;
}
