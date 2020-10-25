#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
	
}


ModelClass::~ModelClass()
{
}

//The Initialize function will call the initialization functions for the vertexand index buffers.

bool ModelClass::Initialize(ID3D11Device * device, std::string modelFilename, LPCWSTR textureFilename)
{
	bool result;
	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}
	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}
	return true;
}

bool ModelClass::LoadTexture(ID3D11Device* device, LPCWSTR filename)
{
	bool result;
	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}
	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
bool ModelClass::LoadModel(std::string path)
{
		
	Assimp::Importer importer;
	importer.FreeScene();
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	
	if (scene == nullptr)
		return false;
		
	//only support 1 mesh for now
	aiMesh* mesh = scene->mMeshes[0];
		
	VertexData* m_vertices = new VertexData[mesh->mNumVertices];
	std::vector<DWORD> m_indices;
	m_vertexCount = mesh->mNumVertices;

		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			m_vertices[i].position.x = mesh->mVertices[i].x;
			m_vertices[i].position.y = mesh->mVertices[i].y;
			m_vertices[i].position.z = mesh->mVertices[i].z;
			//textcoord 0
			if (mesh->mTextureCoords[0]) 
			{
				m_vertices[i].texture.x = (float)mesh->mTextureCoords[0][i].x;
				m_vertices[i].texture.y = (float)mesh->mTextureCoords[0][i].y;
			}
			if (mesh->HasNormals())
			{
				m_vertices[i].normal.x = mesh->mNormals[i].x;
				m_vertices[i].normal.y = mesh->mNormals[i].y;
				m_vertices[i].normal.z = mesh->mNormals[i].z;
			}
		}
		
		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (UINT j = 0; j < face.mNumIndices; j++)
			{
				m_indices.push_back(face.mIndices[j]);
			}
		}
		vertices = m_vertices;
		m_indexCount = m_indices.size();
		indices = new unsigned long[m_indexCount];
		for (UINT i = 0; i < m_indexCount; i++)
		{
			indices[i] = (ULONG)m_indices[i];
		}
	return true;
}

//The Shutdown function will call the shutdown functions for the vertex and index buffers.
void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();
	// Release the vertex and index buffers.
	ShutdownBuffers();
	// Release the model data.
	return;
}

void ModelClass::Frame()
{
	//update vertex buffer
}

//Render is called from the GraphicsClass::Render function.
//This function calls RenderBuffers to put the vertexand index buffers on 
//the graphics pipeline so the color shader will be able to render them.

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	Frame();
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

//GetIndexCount returns the number of indexes in the model.
//The color shader will need this information to draw this model.

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

//The InitializeBuffers function is where we handle creating the vertexand index buffers.
//Usually you would read in a modeland create the buffers from that data file.
//For this tutorial we will just set the points in the vertexand index buffer manually since it is only a single triangle.

bool ModelClass::InitializeBuffers(ID3D11Device* d11device)
{
	this->device = d11device;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;


	// Create the vertex array.
	
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexData) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* ddeviceContext)
{
	if (!deviceContext) 
	{
		this->deviceContext = ddeviceContext;
	}

	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexData);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Render the triangle.
	//deviceContext->DrawIndexed(m_indexCount, 0, 0);
	return;
}
