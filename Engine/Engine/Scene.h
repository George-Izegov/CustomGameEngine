#ifndef _SCENE_H_
#define _SCENE_H_

#include "graphicsclass.h"

#include <d3d11.h>
#include "lightclass.h"
#include <DirectXMath.h>
#include "SimpleText.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "timerclass.h"
#include "AIAgent.h"
#include "AISystem.h"

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
	//virtual bool  Update(int axisX, int axisY, float DeltaSeconds);
	HRESULT Init(ID3D11Device*, ID3D11RenderTargetView*, GraphicsClass*&, int, int, HWND);
	virtual bool Update(ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*, ID3D11DepthStencilView*, ID3D11DepthStencilState*, ImVec4, int axisX, int axisY, Matrix g_pProjectionMatrix, Matrix g_pWorldMatrix, Matrix g_pOrthoMatrix);

private:
	
	CameraClass* m_Camera;
	LightClass* m_Light;
	GraphicsClass* m_Graphics;
	ParticleEmitter* m_SmokeEmitter;
	ParticleEmitter* m_SnowEmitter;
	Gameplane* m_Gameplane;	
	Katamari* m_Katamari;	
	KataVictim* m_Katavictim;
	KataVictim* m_Katavictim2;
	KataVictim* m_Katavictim3;
	KataVictim* m_Katavictim4;

	SimpleText* m_SimpleText;
	int numberOfUnattachedObjects;
	int numberOfAttachedObjects;

	AIAgent* m_AIAgent;
	AISystem* m_AISystem;
};
#endif
