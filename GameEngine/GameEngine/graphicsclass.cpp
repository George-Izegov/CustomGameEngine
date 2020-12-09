#include "graphicsclass.h"
GraphicsClass::GraphicsClass()
{
    m_RenderTexture = 0;
    m_DepthShader = 0;
    m_ShadowShader = 0;
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

    // Create the render to texture object.
    m_RenderTexture = new RenderTextureClass;
    if (!m_RenderTexture)
    {
        return false;
    }

    // Initialize the render to texture object.
    result = m_RenderTexture->Initialize(g_pd3dDevice, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
    if (FAILED(result))
    {
        MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
        return result;
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
        return false;
    }

    // Initialize the shadow shader object.
    result = m_ShadowShader->Initialize(g_pd3dDevice, hwnd);
    if (FAILED(result))
    {
        MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
        return result;
    }

    return result;
}


void GraphicsClass::Shutdown()
{
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

    return;
}

void GraphicsClass::SetRenderable(Gameobject* go, ModelClass* renderable)
{
    m_ModelsPool.push_back(renderable);
    m_GameobjsPool.push_back(go);
}

bool GraphicsClass::RenderSceneToTexture(ID3D11DeviceContext* g_pd3dDeviceContext, LightClass* m_Light, Matrix g_pWorldMatrix, ID3D11RenderTargetView* g_mainRenderTargetView, ID3D11DepthStencilView* g_pDepthStencilView)
{
    XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
    bool result;

    // Set the render target to be the render to texture.
    m_RenderTexture->SetRenderTarget(g_pd3dDeviceContext);

    // Clear the render to texture.
    m_RenderTexture->ClearRenderTarget(g_pd3dDeviceContext, 0.0f, 0.0f, 0.0f, 1.0f);

    // Generate the light view matrix based on the light's position.
    m_Light->GenerateViewMatrix();

    // Get the world matrix from the d3d object.
    worldMatrix = g_pWorldMatrix;

    // Get the view and orthographic matrices from the light object.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetProjectionMatrix(lightProjectionMatrix);

    for (UINT i = 0; i < m_ModelsPool.size(); i++)
    {
        worldMatrix = g_pWorldMatrix;
        worldMatrix = m_GameobjsPool[i]->m_Transform->trs;

        m_ModelsPool[i]->Render(g_pd3dDeviceContext);

        result = m_DepthShader->Render(g_pd3dDeviceContext, m_ModelsPool[i]->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
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

bool GraphicsClass::Render(ID3D11DeviceContext* g_pd3dDeviceContext, IDXGISwapChain* g_pSwapChain, ID3D11RenderTargetView* g_mainRenderTargetView, ID3D11DepthStencilView* g_pDepthStencilView, ImVec4 color, CameraClass* m_Camera, LightClass* m_Light, SimpleText* m_SimpleText, UINT32 numberOfUnattachedObjects, UINT32 numberOfAttachedObjects, Matrix g_pProjectionMatrix, Matrix g_pWorldMatrix, Matrix g_pOrthoMatrix)
{
    XMMATRIX viewMatrix, projectionMatrix, worldMatrix;
    XMMATRIX lightViewMatrix, lightProjectionMatrix;
    bool result = false;

    // First render the scene to a texture.
    result = RenderSceneToTexture(g_pd3dDeviceContext, m_Light, g_pWorldMatrix, g_mainRenderTargetView, g_pDepthStencilView);
    if (!result)
    {
        return false;
    }

    // Clear the buffers to begin the scene.
    BeginScene(g_pd3dDeviceContext, g_pSwapChain, g_mainRenderTargetView, g_pDepthStencilView, color);

    /*wchar_t pretext[200];
    swprintf(pretext, 200, L"Количество свободных объектов на сцене: %u\nКоличество прикрепленных объектов на сцене: %u", numberOfUnattachedObjects, numberOfAttachedObjects);
    m_SimpleText->DrawTextOnScene(480, 60, pretext);*/

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the world, view, and projection matrices from the camera.
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
            lightProjectionMatrix, m_ModelsPool[i]->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(),
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
