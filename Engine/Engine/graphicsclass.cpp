#include "graphicsclass.h"
GraphicsClass::GraphicsClass()
{
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

HRESULT GraphicsClass::Initialize(ID3D11Device* g_pd3dDevice, int screenWidth, int screenHeight, HWND hwnd)
{
	HRESULT result;

	//forward
	// Create the render to texture object.
	m_DepthTexture = new RenderTextureClass;
	if (!m_DepthTexture)
	{
		return E_FAIL;
	}

	// Initialize the render to texture object.
	result = m_DepthTexture->Initialize(g_pd3dDevice, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return E_FAIL;
	}


	// Create the depth shader object.
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader)
	{
		MessageBox(hwnd, L"Could not initialize the m_DepthShader.", L"Error", MB_OK);
		return NULL;
	}

	// Initialize the depth shader object.
	result = m_DepthShader->Initialize(g_pd3dDevice, hwnd);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return result;
	}

	// Create the shadow shader object.
	m_ShadowShader = new ShadowShaderClass;
	if (!m_ShadowShader)
	{
		MessageBox(hwnd, L"Could not initialize the m_ShadowShader.", L"Error", MB_OK);
		return E_FAIL;
	}

	// Initialize the shadow shader object.
	result = m_ShadowShader->Initialize(g_pd3dDevice, hwnd);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return E_FAIL;
	}

	//deferred
	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow)
	{
		return E_FAIL;
	}

	// Initialize the full screen ortho window object.
	result = m_FullScreenWindow->Initialize(g_pd3dDevice, screenWidth, screenHeight);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return E_FAIL;
	}
	m_DeferredBuffers = new DeferredBuffersClass;
	if (!m_DeferredBuffers)
	{
		return E_FAIL;
	}

	// Initialize the deferred buffers object.
	result = m_DeferredBuffers->Initialize(g_pd3dDevice, screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK);
		return E_FAIL;
	}
	m_DeferredShader = new DeferredShaderClass;
	if (!m_DeferredShader)
	{
		return E_FAIL;
	}

	// Initialize the deferred shader object.
	result = m_DeferredShader->Initialize(g_pd3dDevice, hwnd);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the deferred shader object.", L"Error", MB_OK);
		return E_FAIL;
	}

	// Create the light shader object.
	m_PostProcessing = new DeferredPostProcessingClass;
	if (!m_PostProcessing)
	{
		return E_FAIL;
	}

	// Initialize the light shader object.
	result = m_PostProcessing->Initialize(g_pd3dDevice, hwnd);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return E_FAIL;
	}

	return result;
}


void GraphicsClass::Shutdown()
{
	// Forward
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

	return;
}

void GraphicsClass::SetRenderable(Gameobject* go, ModelClass* renderable)
{
	m_ModelsPool.push_back(renderable);
	m_GameobjsPool.push_back(go);
}

bool GraphicsClass::RenderDepthToTexture(ID3D11DeviceContext* g_pd3dDeviceContext, LightClass* m_Light, Matrix g_worldMatrix)
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	bool result;

	// Set the render target to be the render to texture.
	m_DepthTexture->SetRenderTarget(g_pd3dDeviceContext);

	// Clear the render to texture.
	m_DepthTexture->ClearRenderTarget(g_pd3dDeviceContext, 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the world matrix from the d3d object.
	worldMatrix = g_worldMatrix;
	//m_D3D->GetWorldMatrix(worldMatrix);

	// Get the view and orthographic matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	for (UINT i = 0; i < m_ModelsPool.size(); i++)
	{
		//m_D3D->GetWorldMatrix(worldMatrix);
		worldMatrix = m_GameobjsPool[i]->m_Transform->trs;

		m_ModelsPool[i]->Render(g_pd3dDeviceContext);

		result = m_DepthShader->Render(g_pd3dDeviceContext, m_ModelsPool[i]->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		if (!result)
		{
			return false;
		}
	}
	return true;
}

bool GraphicsClass::RenderSceneToTexture(ID3D11DeviceContext* g_pd3dDeviceContext, LightClass* m_Light, CameraClass* m_Camera, Matrix g_pWorldMatrix,
	Matrix g_projectionMatrix, ID3D11RenderTargetView* g_mainRenderTargetView, ID3D11DepthStencilView* g_pDepthStencilView)
{
	XMMATRIX worldMatrix, viewMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	bool result;

	// Set the render buffers to be the render target.
	m_DeferredBuffers->SetRenderTargets(g_pd3dDeviceContext);

	// Clear the render buffers.
	m_DeferredBuffers->ClearRenderTargets(g_pd3dDeviceContext, 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();
	worldMatrix = g_pWorldMatrix;
	// Get the matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	// Get the view and orthographic matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	for (UINT i = 0; i < m_ModelsPool.size(); i++)
	{
		worldMatrix = g_pWorldMatrix;
		worldMatrix = m_GameobjsPool[i]->m_Transform->trs;

		m_ModelsPool[i]->Render(g_pd3dDeviceContext);

		result = m_DeferredShader->Render(g_pd3dDeviceContext, m_ModelsPool[i]->GetIndexCount(), worldMatrix, viewMatrix, g_projectionMatrix,
			lightViewMatrix, lightProjectionMatrix, m_ModelsPool[i]->GetTexture(), m_DepthTexture->GetShaderResourceView(), m_Light->GetPosition(),
			m_Camera->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		if (!result)
		{
			return false;
		}
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, g_pDepthStencilView);

	return true;
}

void GraphicsClass::BeginScene(ID3D11DeviceContext* g_pd3dDeviceContext, IDXGISwapChain* g_pSwapChain, ID3D11RenderTargetView* g_mainRenderTargetView, ID3D11DepthStencilView* g_pDepthStencilView, ImVec4 color)
{
	// Clear the back buffer.
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&color);

	// Clear the depth buffer.
	g_pd3dDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void GraphicsClass::EndScene(IDXGISwapChain* g_pSwapChain)
{
	// Present the back buffer to the screen since rendering is complete.
	if (VSYNC_ENABLED)
	{
		// Lock to screen refresh rate.
		g_pSwapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		g_pSwapChain->Present(0, 0);
	}

	return;
}

bool GraphicsClass::Render(ID3D11DeviceContext* g_pd3dDeviceContext, IDXGISwapChain* g_pSwapChain, ID3D11RenderTargetView* g_mainRenderTargetView,
	ID3D11DepthStencilView* g_pDepthStencilView, ID3D11DepthStencilState* depthStencilState, ImVec4 color, CameraClass* m_Camera, LightClass* m_Light, SimpleText* m_SimpleText,
	UINT32 numberOfUnattachedObjects, UINT32 numberOfAttachedObjects, Matrix g_pProjectionMatrix, Matrix g_pWorldMatrix, Matrix g_pOrthoMatrix)
{
	if (RENDERER == DEFERRED) {
		XMMATRIX baseBiewMatrix, worldMatrix, orthoMatrix;
		bool result = false;

		result = RenderDepthToTexture(g_pd3dDeviceContext, m_Light, g_pWorldMatrix);
		if (!result)
		{
			return false;
		}
		// First render the scene to a texture.
		result = RenderSceneToTexture(g_pd3dDeviceContext, m_Light, m_Camera, g_pWorldMatrix, g_pProjectionMatrix,
			g_mainRenderTargetView, g_pDepthStencilView);
		if (!result)
		{
			return false;
		}

		// Clear the buffers to begin the scene.
		BeginScene(g_pd3dDeviceContext, g_pSwapChain, g_mainRenderTargetView, g_pDepthStencilView, color);

		//wchar_t pretext[200];
		//swprintf(pretext, 200, L"Number of unattached objects: %u\nNumber of attached objects: %u", numberOfUnattachedObjects, numberOfAttachedObjects);
		//m_SimpleText->DrawTextOnScene(480, 60, pretext);

		// Generate the view matrix based on the camera's position.
		m_Camera->Render();

		// Get the world, view, and projection matrices from the camera and d3d objects.
		worldMatrix = g_pWorldMatrix;
		orthoMatrix = g_pOrthoMatrix;
		m_Camera->GetViewMatrix(baseBiewMatrix);


		//m_D3D->TurnZBufferOff();
		g_pd3dDeviceContext->OMSetDepthStencilState(NULL, 1);

		m_FullScreenWindow->Render(g_pd3dDeviceContext);
		// Render models using the ambient light shader.
		result = m_PostProcessing->Render(g_pd3dDeviceContext, m_FullScreenWindow->GetIndexCount(), worldMatrix, baseBiewMatrix, orthoMatrix,
			m_DeferredBuffers->GetShaderResourceViews(0), m_DeferredBuffers->GetShaderResourceViews(1), m_DeferredBuffers->GetShaderResourceViews(2));

		if (!result)
		{
			return false;
		}

		//m_D3D->TurnZBufferOn();
		g_pd3dDeviceContext->OMSetDepthStencilState(depthStencilState, 1);
		// Present the rendered scene to the screen.
		EndScene(g_pSwapChain);
		return true;
	}
	else
	{
		XMMATRIX viewMatrix, projectionMatrix, worldMatrix;
		XMMATRIX lightViewMatrix, lightProjectionMatrix;
		bool result = false;


		// First render the scene to a texture.
		result = RenderDepthToTexture(g_pd3dDeviceContext, m_Light, g_pWorldMatrix);
		if (!result)
		{
			return false;
		}

		// Clear the buffers to begin the scene.
		BeginScene(g_pd3dDeviceContext, g_pSwapChain, g_mainRenderTargetView, g_pDepthStencilView, color);

		//wchar_t pretext[200];
		//swprintf(pretext, 200, L"Number of unattached objects: %u\nNumber of attached objects: %u", numberOfUnattachedObjects, numberOfAttachedObjects);
		//m_SimpleText->DrawTextOnScene(240, 50, pretext);

		// Generate the view matrix based on the camera's position.
		m_Camera->Render();

		// Get the world, view, and projection matrices from the camera and d3d objects.
		m_Camera->GetViewMatrix(viewMatrix);
		worldMatrix = g_pWorldMatrix;
		projectionMatrix = g_pProjectionMatrix;

		// Get the light's view and projection matrices from the light object.
		m_Light->GetViewMatrix(lightViewMatrix);
		m_Light->GetProjectionMatrix(lightProjectionMatrix);

		for (UINT i = 0; i < m_ModelsPool.size(); i++)
		{
			worldMatrix = m_GameobjsPool[i]->m_Transform->trs;

			m_ModelsPool[i]->Render(g_pd3dDeviceContext);

			// Render models using the ambient light shader.
			result = m_ShadowShader->Render(g_pd3dDeviceContext, m_ModelsPool[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
				lightProjectionMatrix, m_ModelsPool[i]->GetTexture(), m_DepthTexture->GetShaderResourceView(), m_Light->GetPosition(),
				m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
			{
				return false;
			}
		}

		// Present the rendered scene to the screen.
		EndScene(g_pSwapChain);
		return true;
	}
}