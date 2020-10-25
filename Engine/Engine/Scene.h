#ifndef _SCENE_H_
#define _SCENE_H_

#include "graphicsclass.h"
#include "Katamari.h"
#include "KataVictim.h"
#include "Gameplane.h"
#include <d3d11.h>
#include "lightclass.h"
#include <DirectXMath.h>
#include "SimpleText.h"

using namespace std;
using namespace DirectX;

class Scene  {
public:
	Scene();
	~Scene();

	virtual void Load();
	virtual void  Unload();
	bool Init(int, int, HWND);
	virtual bool  Update(int axisX, int axisY);

private:
	
	CameraClass* m_Camera;
	LightClass* m_Light;
	GraphicsClass* m_Graphics;
	Gameplane* m_Gameplane;	
	Katamari* m_Katamari;	
	KataVictim* m_Katavictim;
	KataVictim* m_Katavictim2;
	KataVictim* m_Katavictim3;
	SimpleText* m_SimpleText;
	int numberOfUnattachedObjects;
	int numberOfAttachedObjects;
};
#endif