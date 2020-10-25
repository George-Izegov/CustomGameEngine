////////////////////////////////////////////////////////////////////////////////
// Filename: shadowshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SHADOWSHADERCLASS_H_
#define _SHADOWSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: ShadowShaderClass
////////////////////////////////////////////////////////////////////////////////
class ShadowShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};
	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float cameraPadding;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor; //16
		XMFLOAT4 diffuseColor; //16
		XMFLOAT4 specularColor; //16
		float specularPower; //4
		XMFLOAT3 bufferPadding;//12
	};

	struct LightBufferType2
	{
		XMFLOAT3 lightPosition;
		float lightPadding;
	};

public:
	ShadowShaderClass();
	ShadowShaderClass(const ShadowShaderClass&);
	~ShadowShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4,XMFLOAT3, XMFLOAT4,float);

private:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCWSTR);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4,XMFLOAT3, XMFLOAT4,float);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightBuffer2;
	ID3D11Buffer* m_cameraBuffer;
};

#endif