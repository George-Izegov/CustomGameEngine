#include "graphicsclass.h"
GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	//Forward
	m_DepthTexture = 0;
	m_DepthShader = 0;
	m_ShadowShader = 0;
	//Deferred
	m_DeferredShader = 0;
	m_DeferredBuffers = 0;
	m_PostProcessing = 0;
	m_FullScreenWindow = 0;
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
	//Forward
#pragma region Forward Renderer Initialize
	m_DepthTexture = new RenderTextureClass;
	if (!m_DepthTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_DepthTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
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

#pragma endregion

	//Deferred
#pragma region Deferred Renderer Initialize
	// Create the full screen ortho window object.
	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow)
	{
		return false;
	}

	// Initialize the full screen ortho window object.
	result = m_FullScreenWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}


	// Create the deferred buffers object.
	m_DeferredBuffers = new DeferredBuffersClass;
	if (!m_DeferredBuffers)
	{
		return false;
	}

	// Initialize the deferred buffers object.
	result = m_DeferredBuffers->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK);
		return false;
	}

	// Create the deferred shader object.
	m_DeferredShader = new DeferredShaderClass;
	if (!m_DeferredShader)
	{
		return false;
	}

	// Initialize the deferred shader object.
	result = m_DeferredShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the deferred shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_PostProcessing = new DeferredPostProcessingClass;
	if (!m_PostProcessing)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_PostProcessing->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}
#pragma endregion

	return true;
}


void GraphicsClass::Shutdown()
{
	//Forward
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
	if (m_DepthTexture)
	{
		m_DepthTexture->Shutdown();
		delete m_DepthTexture;
		m_DepthTexture = 0;
	}
	//Deferred
	// Release the light shader object.
	if (m_PostProcessing)
	{
		m_PostProcessing->Shutdown();
		delete m_PostProcessing;
		m_PostProcessing = 0;
	}
	// Release the deferred shader object.
	if (m_DeferredShader)
	{
		m_DeferredShader->Shutdown();
		delete m_DeferredShader;
		m_DeferredShader = 0;
	}

	// Release the deferred buffers object.
	if (m_DeferredBuffers)
	{
		m_DeferredBuffers->Shutdown();
		delete m_DeferredBuffers;
		m_DeferredBuffers = 0;
	}
	// Release the full screen ortho window object.
	if (m_FullScreenWindow)
	{
		m_FullScreenWindow->Shutdown();
		delete m_FullScreenWindow;
		m_FullScreenWindow = 0;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

void GraphicsClass::SetRenderable(Gameobject* go, ModelClass* renderable)
{
	m_ModelsPool.push_back(renderable);
	m_GameobjsPool.push_back(go);
}

//For forward shading model
bool GraphicsClass::RenderDepthToTexture(LightClass* m_Light)
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	bool result;

	// Set the render target to be the render to texture.
	m_DepthTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// Clear the render to texture.
	m_DepthTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

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

//For deferred shading model
bool GraphicsClass::RenderSceneToTexture(CameraClass* m_Camera, LightClass* m_Light)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix;
	bool result;

	// Set the render buffers to be the render target.
	m_DeferredBuffers->SetRenderTargets(m_D3D->GetDeviceContext());

	// Clear the render buffers.
	m_DeferredBuffers->ClearRenderTargets(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	for (UINT i = 0; i < m_ModelsPool.size(); i++)
	{
		m_D3D->GetWorldMatrix(worldMatrix);
		worldMatrix = m_GameobjsPool[i]->m_Transform->trs;

		m_ModelsPool[i]->Render(m_D3D->GetDeviceContext());

		result = m_DeferredShader->Render(m_D3D->GetDeviceContext(), m_ModelsPool[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
			lightViewMatrix, lightProjectionMatrix, m_ModelsPool[i]->GetTexture(), m_DepthTexture->GetShaderResourceView(), m_Light->GetPosition(), m_Camera->GetPosition(),
			m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
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

bool GraphicsClass::Render(CameraClass* m_Camera, LightClass* m_Light, SimpleText* m_SimpleText, UINT32 numberOfUnattachedObjects, UINT32 numberOfAttachedObjects)
{
	if (RENDERER == DEFERRED) {
		XMMATRIX baseBiewMatrix, worldMatrix, orthoMatrix;
		bool result = false;

		result = RenderDepthToTexture(m_Light);
		if (!result)
		{
			return false;
		}
		// First render the scene to a texture.
		result = RenderSceneToTexture(m_Camera, m_Light);
		if (!result)
		{
			return false;
		}

		// Clear the buffers to begin the scene.
		m_D3D->BeginScene(.0f, .0f, .0f, 1.0f);

		wchar_t pretext[200];
		swprintf(pretext, 200, L"Количество свободных объектов на сцене: %u\nКоличество прикрепленных объектов на сцене: %u", numberOfUnattachedObjects, numberOfAttachedObjects);
		m_SimpleText->DrawTextOnScene(480, 60, pretext);

		// Generate the view matrix based on the camera's position.
		m_Camera->Render();

		// Get the world, view, and projection matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);
		m_Camera->GetViewMatrix(baseBiewMatrix);


		m_D3D->TurnZBufferOff();


		m_FullScreenWindow->Render(m_D3D->GetDeviceContext());
		// Render models using the ambient light shader.
		result = m_PostProcessing->Render(m_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseBiewMatrix, orthoMatrix,
			m_DeferredBuffers->GetShaderResourceViews(0), m_DeferredBuffers->GetShaderResourceViews(1), m_DeferredBuffers->GetShaderResourceViews(2));

		if (!result)
		{
			return false;
		}

		m_D3D->TurnZBufferOn();
		// Present the rendered scene to the screen.
		m_D3D->EndScene();
		return true;
	}
	else 
	{
		XMMATRIX viewMatrix, projectionMatrix, worldMatrix;
		XMMATRIX lightViewMatrix, lightProjectionMatrix;
		bool result = false;


		// First render the scene to a texture.
		result = RenderDepthToTexture(m_Light);
		if (!result)
		{
			return false;
		}

		// Clear the buffers to begin the scene.
		m_D3D->BeginScene(.0f, .0f, .0f, 1.0f);

		wchar_t pretext[200];
		swprintf(pretext, 200, L"Number of unattached objects: %u\nNumber of attached objects: %u", numberOfUnattachedObjects, numberOfAttachedObjects);
		m_SimpleText->DrawTextOnScene(240, 50, pretext);

		// Generate the view matrix based on the camera's position.
		m_Camera->Render();

		// Get the world, view, and projection matrices from the camera and d3d objects.
		m_Camera->GetViewMatrix(viewMatrix);
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetProjectionMatrix(projectionMatrix);

		// Get the light's view and projection matrices from the light object.
		m_Light->GetViewMatrix(lightViewMatrix);
		m_Light->GetProjectionMatrix(lightProjectionMatrix);

		for (UINT i = 0; i < m_ModelsPool.size(); i++)
		{
			worldMatrix = m_GameobjsPool[i]->m_Transform->trs;

			m_ModelsPool[i]->Render(m_D3D->GetDeviceContext());

			// Render models using the ambient light shader.
			result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_ModelsPool[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
				lightProjectionMatrix, m_ModelsPool[i]->GetTexture(), m_DepthTexture->GetShaderResourceView(), m_Light->GetPosition(),
				m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
			{
				return false;
			}
		}

		// Present the rendered scene to the screen.
		m_D3D->EndScene();
		return true;
	}
}


