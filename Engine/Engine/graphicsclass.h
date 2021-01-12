#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_
#include <windows.h>
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"
#include "Gameobject.h"
#include "SimpleText.h"

#include "rendertextureclass.h"
#include "shadowshaderclass.h"
#include "deferredshaderclass.h"
#include "deferredbuffersclass.h"
#include "deferredpostprocessingclass.h"
#include "orthowindowclass.h"
#include "depthshaderclass.h"

enum RendererType
{
	FORWARD,DEFERRED
};

const RendererType RENDERER = DEFERRED;
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 200.0f;
const float SCREEN_NEAR = 1.0f;

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

	D3DClass* m_D3D;
	bool Render(CameraClass*, LightClass*, SimpleText*, UINT32, UINT32);
	void SetRenderable(Gameobject*, ModelClass*);
	bool RenderDepthToTexture(LightClass*);
	bool RenderSceneToTexture(CameraClass*, LightClass*);

	std::vector<Gameobject*> m_GameobjsPool;
	std::vector<ModelClass*> m_ModelsPool;
	
	//Forward
	RenderTextureClass* m_DepthTexture;
	DepthShaderClass* m_DepthShader;
	ShadowShaderClass* m_ShadowShader;
	
	//Deferred	
	OrthoWindowClass* m_FullScreenWindow;
	DeferredShaderClass* m_DeferredShader;
	DeferredBuffersClass* m_DeferredBuffers;
	DeferredPostProcessingClass* m_PostProcessing;
};
#endif