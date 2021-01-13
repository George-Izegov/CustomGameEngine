#ifndef _SCENE_H_
#define _SCENE_H_

#include "graphicsclass.h"

#include <d3d11.h>
#include "lightclass.h"
#include <DirectXMath.h>
#include "SimpleText.h"

#include "Katamari.h"
#include "KataVictim.h"
#include "Gameplane.h"

using namespace std;
using namespace DirectX;

class Scene  {
public:
	Scene();
	~Scene();

	virtual void Load();
	virtual void  Unload();

	HRESULT Init(ID3D11Device*, ID3D11RenderTargetView*, int, int, HWND);
	virtual bool Update(ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*, ID3D11DepthStencilView*, ID3D11DepthStencilState*, ImVec4, int axisX, int axisY, Matrix g_pProjectionMatrix, Matrix g_pWorldMatrix, Matrix g_pOrthoMatrix);


private:
	
	CameraClass* m_Camera;
	LightClass* m_Light;
	GraphicsClass* m_Graphics;
	Gameplane* m_Gameplane;	
	Katamari* m_Katamari;	
	KataVictim* m_Katavictim;
	KataVictim* m_Katavictim2;
	KataVictim* m_Katavictim3;
	KataVictim* m_Katavictim4;

	SimpleText* m_SimpleText;
	int numberOfUnattachedObjects;
	int numberOfAttachedObjects;
};
#endif
