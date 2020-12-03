#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_
#include <windows.h>
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"
#include "Gameobject.h"
#include "SimpleText.h"
#include "ParticleSystem/particlesystemclass.h"

#include "rendertextureclass.h"
#include "depthshaderclass.h"
#include "shadowshaderclass.h"
#include "ParticleSystem/particleshaderclass.h"


const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

const int SHADOWMAP_WIDTH = 2048;
const int SHADOWMAP_HEIGHT = 2048;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

	D3DClass* m_D3D;
	bool Render(float DeltaSeconds, CameraClass*, LightClass*, SimpleText*, UINT32, UINT32);
	void SetRenderable(Gameobject*, ModelClass*);
	void SetRenderableEmitter(ParticleSystemClass*);
	bool RenderSceneToTexture(LightClass*);

	std::vector<Gameobject*> m_GameobjsPool;
	std::vector<ModelClass*> m_ModelsPool;
	std::vector<ParticleSystemClass*> m_Emitters;

	RenderTextureClass* m_RenderTexture;
	DepthShaderClass* m_DepthShader;
	ShadowShaderClass* m_ShadowShader;

	ParticleShaderClass* m_ParticleShader;
};
#endif