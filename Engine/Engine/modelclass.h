#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "textureclass.h"
#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

using namespace std;
using namespace DirectX;

//Primitive mesh class
class ModelClass
{
private:
	struct VertexData
	{
		//do not change order!!!
		//or change it also at shader .cpp
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();
	bool Initialize(ID3D11Device*, std::string,LPCWSTR);
	void Shutdown();
	void Frame();

	ID3D11ShaderResourceView* GetTexture();

	void Render(ID3D11DeviceContext*);
	int GetIndexCount();

private:
	bool LoadTexture(ID3D11Device*, LPCWSTR);
	void ReleaseTexture();

	bool LoadModel(std::string path);
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	ID3D11Device* device;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	TextureClass* m_Texture;
public:
	int m_vertexCount, m_indexCount;
	unsigned long* indices;
	VertexData* vertices;
	ID3D11DeviceContext* deviceContext;
};
#endif
