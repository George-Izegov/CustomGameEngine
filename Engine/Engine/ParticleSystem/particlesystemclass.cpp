////////////////////////////////////////////////////////////////////////////////
// Filename: particlesystemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "particlesystemclass.h"
#include "ParticleConstants.h"

inline float RandomVariance(float median, float variance)
{
	float fUnitRandomValue = (float)rand() / (float)RAND_MAX;
	float fRange = variance * fUnitRandomValue;
	return median - variance + (2.0f * fRange);
}

ParticleSystemClass::ParticleSystemClass()
{
	m_vertices = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_computeBuffer = 0;
	m_emitterBuffer = 0;
	m_worldMatrix = Matrix::Identity;
}


ParticleSystemClass::ParticleSystemClass(const ParticleSystemClass& other)
{
}


ParticleSystemClass::~ParticleSystemClass()
{
}


bool ParticleSystemClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename, Emitter& EmitterParameters)
{
	bool result;

	// Load the texture that is used for the particles.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	m_EmitterData = EmitterParameters;

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers(device, deviceContext);
	if(!result)
		return false;

	return true;
}


void ParticleSystemClass::Shutdown()
{
	// Release the buffers.
	ShutdownBuffers();

}


void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext, Vector3 cameraPosition)
{
	CreateEmitterWorldMatrix(cameraPosition);
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ParticleSystemClass::GetIndexCount()
{
	return m_indexCount;
}


bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	unsigned int* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_EmitterData.MaxParticles = m_EmitterData.ParticlePerSec * (m_EmitterData.ParticlesLifetime);

	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = m_EmitterData.MaxParticles * 4;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_EmitterData.MaxParticles * 6;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new VertexType[m_vertexCount];

	// Create the index array.
	indices = new unsigned int[m_indexCount];

	m_particlesGPU = new ParticleGPU[m_EmitterData.MaxParticles];

	for (i = 0; i < m_EmitterData.MaxParticles; i++)
	{
		m_particlesGPU[i].randFloat = (rand() % 1000) / 1000.0f;
		m_particlesGPU[i].isAlive = false;
	}

	// Initialize the index array.
	UINT base = 0, ind = 0;
	for (int i = 0; i < m_EmitterData.MaxParticles; i++)
	{
		indices[0 + ind] = base + 0;
		indices[1 + ind] = base + 1;
		indices[2 + ind] = base + 2;

		indices[3+ ind] = base + 2;
		indices[4 + ind] = base + 1;
		indices[5+ ind] = base + 3;

		base += 4;
		ind += 6;
	}

	// Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	vertexBufferDesc.StructureByteStride = sizeof(VertexType);

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
		return false;

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(int) * m_indexCount;
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
	if(FAILED(result))
		return false;

	// Release the index array since it is no longer needed.
	delete [] indices;
	indices = 0;

	D3D11_SUBRESOURCE_DATA subresource;
	subresource.pSysMem = m_particlesGPU;
	subresource.SysMemPitch = 0;
	subresource.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(ParticleGPU) * m_EmitterData.MaxParticles;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(ParticleGPU);

	device->CreateBuffer(&desc, &subresource, &m_computeBuffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
	uav.Format = DXGI_FORMAT_UNKNOWN;
	uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uav.Buffer.FirstElement = 0;
	uav.Buffer.NumElements = m_EmitterData.MaxParticles;
	uav.Buffer.Flags = 0;

	result = device->CreateUnorderedAccessView(m_computeBuffer, &uav, &computeUAV);

	uav.Buffer.NumElements = m_vertexCount;
	result = device->CreateUnorderedAccessView(m_vertexBuffer, &uav, &vertexUAV);

	D3D11_SUBRESOURCE_DATA subresource1;
	subresource1.pSysMem = &m_EmitterData;
	subresource1.SysMemPitch = 0;
	subresource1.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC desc1;
	desc1.ByteWidth = sizeof(Emitter);
	desc1.Usage = D3D11_USAGE_DEFAULT;
	desc1.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc1.CPUAccessFlags = 0;
	desc1.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc1.StructureByteStride = sizeof(Emitter);

	result = device->CreateBuffer(&desc1, &subresource1, &m_emitterBuffer);

	uav.Buffer.NumElements = 1;
	result = device->CreateUnorderedAccessView(m_emitterBuffer, &uav, &emitterUAV);

	FillRandomTexture(device);

	return true;
}


void ParticleSystemClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}




void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
    stride = sizeof(VertexType); 
	offset = 0;

	ID3D11Buffer* nullBuffer = NULL;
	ID3D11UnorderedAccessView* nullUAV = NULL;

	ID3D11ShaderResourceView* srvs[] = { m_pRandomTextureSRV };
	deviceContext->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

	deviceContext->CSSetUnorderedAccessViews(2, 1, &emitterUAV, NULL);

	// Устанавливаем доступ к буферу частиц у вычислительного шейдера
	deviceContext->CSSetUnorderedAccessViews(1, 1, &computeUAV, NULL);

	// Убираем доступ вершинного шейдера к буферу позиций
	deviceContext->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);

	// Устанавливаем доступ вычислительного шейдера к вертексному буферу
	deviceContext->CSSetUnorderedAccessViews(0, 1, &vertexUAV, NULL);

	// Вызываем вычислительный шейдер
	deviceContext->Dispatch(1, 1, 1);

	// Убираем доступ вычислительного шейдера к буферу позиций
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ParticleSystemClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
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

ID3D11ShaderResourceView* ParticleSystemClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void ParticleSystemClass::ReleaseTexture()
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

void ParticleSystemClass::CreateEmitterWorldMatrix(Vector3 camPos)
{
	// перемещаем мировую матрицу на позицию эмитера
	m_worldMatrix = XMMatrixTranslation(m_EmitterData.EmitterPosition.x, m_EmitterData.EmitterPosition.y, m_EmitterData.EmitterPosition.z);
}


// Populate a texture with random numbers (used for the emission of particles)
void ParticleSystemClass::FillRandomTexture(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = 1024;
	desc.Height = 1024;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	float* values = new float[desc.Width * desc.Height * 4];
	float* ptr = values;
	for (UINT i = 0; i < desc.Width * desc.Height; i++)
	{
		ptr[0] = RandomVariance(0.0f, 1.0f);
		ptr[1] = RandomVariance(0.0f, 1.0f);
		ptr[2] = RandomVariance(0.0f, 1.0f);
		ptr[3] = RandomVariance(0.0f, 1.0f);
		ptr += 4;
	}

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = values;
	data.SysMemPitch = desc.Width * 16;
	data.SysMemSlicePitch = 0;

	device->CreateTexture2D(&desc, &data, &m_pRandomTexture);

	delete[] values;

	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Format = desc.Format;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	srv.Texture2D.MostDetailedMip = 0;

	device->CreateShaderResourceView(m_pRandomTexture, &srv, &m_pRandomTextureSRV);
}
