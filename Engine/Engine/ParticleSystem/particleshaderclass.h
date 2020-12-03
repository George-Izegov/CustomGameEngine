#ifndef _PARTICLESHADERCLASS_H_
#define _PARTICLESHADERCLASS_H_


#include <d3d11.h>
#include "SimpleMath.h"
#include <d3dcompiler.h>
#include <fstream>
using namespace std;
using namespace DirectX::SimpleMath;

////////////////////////////////////////////////////////////////////////////////
// Class name: ParticleShaderClass
////////////////////////////////////////////////////////////////////////////////
class ParticleShaderClass
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	struct CommonDataForParticleSystem
	{
		Vector3 cameraPosition;
		float DeltaSeconds;
	};

public:
	ParticleShaderClass();
	ParticleShaderClass(const ParticleShaderClass&);
	~ParticleShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(float DeltaSeconds, ID3D11DeviceContext* deviceContext, int indexCount, Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix, Vector3 cameraPosition, ID3D11ShaderResourceView* texture);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(float DeltaSeconds, ID3D11DeviceContext* deviceContext, Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix, Vector3 cameraPosition, ID3D11ShaderResourceView* texture);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11ComputeShader* m_computeShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer, *m_commonBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif