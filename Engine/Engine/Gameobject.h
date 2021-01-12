#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "modelclass.h"
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

		XMFLOAT3 translation;
		XMFLOAT3 rotation;
		XMFLOAT3 scale;
	};

	virtual void Load();
	virtual void  Unload();
	virtual HRESULT Init(HWND hwnd, ID3D11Device*);
	virtual HRESULT Init(HWND, LPCWSTR, LPCWSTR, Vector3, ID3D11Device*);
	virtual bool  Update();

	bool IsCloser(Gameobject* other, float dist);
	Vector3 Distance(Gameobject* other);

	//local transform
	Transform* m_Transform;
	Matrix m_Translation;
	Matrix m_Rotation;
	Matrix m_Scale;
protected:
	ID3D11Device* device;
};
#endif
