#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}


void CameraClass::Follow(Matrix trs,Vector3 offset)

{
	Vector3 translation;
	Quaternion rotation;
	Vector3 scale;
	trs.Decompose(scale, rotation, translation);
	translation += offset;
	SetPosition(translation.x,translation.y,translation.z);
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(yaw, pitch, roll);

	XMVECTOR v_lookAt = XMLoadFloat3(&lookAt);
	XMVECTOR v_up = XMLoadFloat3(&up);
	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	//D3DXVec3TransformCoord(&lookAt, &rotationMatrix);
	v_lookAt = XMVector3TransformCoord(v_lookAt, rotationMatrix);
	//D3DXVec3TransformCoord(&up, &up, &rotationMatrix);
	v_up = XMVector3TransformCoord(v_up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	v_lookAt = XMLoadFloat3(&position) + v_lookAt;

	// Finally create the view matrix from the three updated vectors.
	//D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
	m_viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&position), v_lookAt, v_up);

	return;
}
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}
void CameraClass::RenderBaseViewMatrix()
{
	XMFLOAT3 up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(yaw, pitch, roll);

	XMVECTOR v_lookAt = XMLoadFloat3(&lookAt);
	XMVECTOR v_up = XMLoadFloat3(&up);
	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.

	v_lookAt = XMVector3TransformCoord(v_lookAt, rotationMatrix);
	v_up = XMVector3TransformCoord(v_up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	v_lookAt = XMLoadFloat3(&position) + v_lookAt;

	// Finally create the base view matrix from the three updated vectors.
	m_baseViewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&position), v_lookAt, v_up);

	return;
}


void CameraClass::GetBaseViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_baseViewMatrix;
	return;
}
