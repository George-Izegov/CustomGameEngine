#include "graphicsclass.h"
GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_RenderTexture = 0;
	m_DepthShader = 0;
	m_ShadowShader = 0;
	m_ParticleShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the depth shader object.
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader)
	{
		return false;
	}

	// Initialize the depth shader object.
	result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the shadow shader object.
	m_ShadowShader = new ShadowShaderClass;
	if (!m_ShadowShader)
	{
		return false;
	}

	// Initialize the shadow shader object.
	result = m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the particle shader object.
	m_ParticleShader = new ParticleShaderClass;
	if (!m_ParticleShader)
	{
		return false;
	}

	// Initialize the particle shader object.
	result = m_ParticleShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the particle shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{

	// Release the particle shader object.
	if (m_ParticleShader)
	{
		m_ParticleShader->Shutdown();
		delete m_ParticleShader;
		m_ParticleShader = 0;
	}

	// Release the shadow shader object.
	if (m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}

	// Release the depth shader object.
	if (m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	// Release the render to texture object.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

//perform update
//obsolete from 8.04.2020
//use scene update
bool GraphicsClass::Frame()
{
	return true;
}

void GraphicsClass::SetRenderable(Gameobject* go,ModelClass* renderable)
{
	m_ModelsPool.push_back(renderable);
	m_GameobjsPool.push_back(go);
}

void GraphicsClass::SetRenderableEmitter(ParticleSystemClass* emitter)
{
	m_Emitters.push_back(emitter);
}

bool GraphicsClass::RenderSceneToTexture(LightClass* m_Light)
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	bool result;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the world matrix from the d3d object.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Get the view and orthographic matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	for (UINT i = 0; i < m_ModelsPool.size(); i++)
	{
		m_D3D->GetWorldMatrix(worldMatrix);
		worldMatrix = m_GameobjsPool[i]->m_Transform->trs;

		m_ModelsPool[i]->Render(m_D3D->GetDeviceContext());

		result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_ModelsPool[i]->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		if (!result)
		{
			return false;
		}
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::Render(float DeltaSeconds, CameraClass* m_Camera, LightClass* m_Light, SimpleText* m_SimpleText, UINT32 numberOfUnattachedObjects, UINT32 numberOfAttachedObjects)
{
	XMMATRIX viewMatrix, projectionMatrix, worldMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	bool result = false;

	m_D3D->SetBackBufferRenderTarget();
	// First render the scene to a texture.
	result = RenderSceneToTexture(m_Light);
	if (!result)
	{
		return false;
	}
	
	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(.0f, .0f, .0f, 1.0f);

	UINT32 FPS = (UINT32) 1 / DeltaSeconds;
	wchar_t pretext[200];
	swprintf(pretext, 200, L"FPS: %i\nКоличество свободных объектов на сцене: %u\nКоличество прикрепленных объектов на сцене: %u", FPS, numberOfUnattachedObjects, numberOfAttachedObjects);
	m_SimpleText->DrawTextOnScene(480, 60, pretext);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	
	// Get the light's view and projection matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	m_D3D->EnableAlphaBlending();
	
	for (UINT i = 0; i < m_Emitters.size(); i++)
	{
		// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Emitters[i]->Render(m_D3D->GetDeviceContext(), m_Camera->GetPosition());

		worldMatrix = m_Emitters[i]->GetWorldMatrix();
		// Render the model using the texture shader.
		result = m_ParticleShader->Render(DeltaSeconds, m_D3D->GetDeviceContext(), m_Emitters[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
			m_Camera->GetPosition(), m_Emitters[i]->GetTexture());

		if (!result)
			return false;
	}

	m_D3D->DisableAlphaBlending();

	for (UINT i = 0; i < m_ModelsPool.size(); i++)
	{
		worldMatrix = m_GameobjsPool[i]->m_Transform->trs;

		m_ModelsPool[i]->Render(m_D3D->GetDeviceContext());

		// Render models using the ambient light shader.
		result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_ModelsPool[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
			lightProjectionMatrix, m_ModelsPool[i]->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(),
			m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),m_Camera->GetPosition(),m_Light->GetSpecularColor(),m_Light->GetSpecularPower());
		if (!result)
		{
			return false;
		}
	}
	
	// Present the rendered scene to the screen.
	m_D3D->EndScene();
	return true;
}


