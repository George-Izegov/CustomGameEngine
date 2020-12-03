#ifndef _PARTICLESYSTEMCLASS_H_
#define _PARTICLESYSTEMCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include "../textureclass.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

struct Emitter
{
	Vector3 EmitterPosition;
	Vector3 EffectedForce;
	int ParticlePerSec;
	float ParticleSize;
	Vector3 ParticleSpawnDeviation;
	Vector3 StartVelocity;
	Vector3 VelDeviation;
	Vector4 ParticleColor;
	Vector4 ColorDeviation;
	float ParticlesLifetime;
	float LifetimeDeviation;
	float ParticlesMass;
	float MassDeviation;

	int MaxParticles;
private:
	float WaitTimeForSpawnParticle = 0.0f;
};

class ParticleEmitter
{
private:

	struct ParticleGPU
	{
		Vector3 velocity;
		float lifetime;
		float mass;
		bool isAlive;
		float randFloat;
	};

	// position(v3), color(v4), cornerPos(v3), sincos(v2), texcoord(v2)
	struct VertexType
	{
		float p[14];
	};

public:
	ParticleEmitter();
	ParticleEmitter(const ParticleEmitter&);
	~ParticleEmitter();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename, Emitter& EmitterParameters);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext, Vector3 cameraPosition);

	int GetIndexCount();

	Matrix GetWorldMatrix() { return m_worldMatrix; }

	ID3D11ShaderResourceView* GetTexture();

private:

	bool InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void ShutdownBuffers();

	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	void CreateEmitterWorldMatrix(Vector3 cameraPosition);

	void FillRandomTexture(ID3D11Device* device);

	bool LoadTexture(ID3D11Device* device, WCHAR* textureFilename);
	void ReleaseTexture();

private:

	int m_vertexCount, m_indexCount;
	VertexType* m_vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer, *m_computeBuffer, *m_emitterBuffer;
	Matrix m_worldMatrix;
	TextureClass* m_Texture;
	ID3D11UnorderedAccessView* computeUAV, *emitterUAV;
	ID3D11UnorderedAccessView* vertexUAV;
	ParticleGPU* m_particlesGPU;
	Emitter m_EmitterData;

	ID3D11Texture2D* m_pRandomTexture;
	ID3D11ShaderResourceView* m_pRandomTextureSRV;
};

#endif