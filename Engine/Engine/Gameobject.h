#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "modelclass.h"
#include "d3dclass.h"
#include <d3d11.h>
#include "SimpleMath.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
class Gameobject 
{
public:
	Gameobject();
	~Gameobject();
	struct Transform
	{
		Matrix trs;

		/*XMFLOAT3 translation;
		XMFLOAT3 rotation;
		XMFLOAT3 scale;*/
	};

	virtual void Load();
	virtual void  Unload();
	virtual bool Init(HWND,D3DClass*);
	virtual bool Init(HWND, std::string, LPCWSTR, Vector3, D3DClass*);
	virtual bool  Update();

	bool IsCloser(Gameobject* other, float dist);
	Vector3 Distance(Gameobject* other);
	//local transform
	Transform* m_Transform;
	Matrix m_Translation;
	Matrix m_Rotation;
	Matrix m_Scale;
protected:
	D3DClass* m_D3D;
};
#endif