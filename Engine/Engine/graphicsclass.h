#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_
#include <windows.h>
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"
#include "Gameobject.h"
#include "SimpleText.h"
#include "ParticleSystem/ParticleEmitter.h"

#include "rendertextureclass.h"

#include "shadowshaderclass.h"
#include "deferredshaderclass.h"
#include "deferredbuffersclass.h"
#include "deferredpostprocessingclass.h"
#include "orthowindowclass.h"
#include "depthshaderclass.h"
#include "imgui/imgui.h"
#include "ParticleSystem/particleshaderclass.h"

enum RendererType
{
	FORWARD,DEFERRED
};

const RendererType RENDERER = FORWARD;


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

	HRESULT Initialize(ID3D11Device*, int, int, HWND);
	void Shutdown();
	bool Frame();


	bool Render(float DeltaTime, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*, ID3D11DepthStencilView*, ID3D11DepthStencilState*, ImVec4, CameraClass*, LightClass*, SimpleText*, UINT32, UINT32, Matrix, Matrix, Matrix);
	void SetRenderable(Gameobject*, ModelClass*);

	void SetRenderableEmitter(ParticleEmitter*);	bool RenderDepthToTexture(ID3D11DeviceContext*,LightClass*,Matrix);
	bool RenderSceneToTexture(ID3D11DeviceContext*, LightClass*, CameraClass*, Matrix, Matrix, ID3D11RenderTargetView*, ID3D11DepthStencilView*);
	void BeginScene(ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*, ID3D11DepthStencilView*, ImVec4);
	void EndScene(IDXGISwapChain*);

	HRESULT InitBlendStates(ID3D11Device*);
	void EnableAlphaBlending(ID3D11DeviceContext* g_pd3dDeviceContext);
	void DisableAlphaBlending(ID3D11DeviceContext* g_pd3dDeviceContext);

	std::vector<Gameobject*> m_GameobjsPool;
	std::vector<ModelClass*> m_ModelsPool;
	std::vector<ParticleEmitter*> m_Emitters;
	// Forward
	RenderTextureClass* m_DepthTexture;	RenderTextureClass* m_RenderTexture;
	DepthShaderClass* m_DepthShader;
	ShadowShaderClass* m_ShadowShader;
	ParticleShaderClass* m_ParticleShader;

	//Deferred	
	OrthoWindowClass* m_FullScreenWindow;
	DeferredShaderClass* m_DeferredShader;
	DeferredBuffersClass* m_DeferredBuffers;
	DeferredPostProcessingClass* m_PostProcessing;

	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;
};
#endif
