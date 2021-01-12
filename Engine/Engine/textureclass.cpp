#include "textureclass.h"


TextureClass::TextureClass()
{
	m_texture = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

HRESULT TextureClass::Initialize(ID3D11Device* device, LPCWSTR filename)
{
	HRESULT result = S_OK;



	// Load the texture in.
	//CreateShaderResourceView(device, filename, NULL, NULL, &m_texture, NULL);
	ScratchImage image;
	//LoadFromTGAFile()
	DirectX::TexMetadata txMeta;
	txMeta.format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;//WIC_FLAGS_FORCE_SRGB;
	//txMeta.
	//result = LoadFromTGAFile(filename, &txMeta, image);
	result = LoadFromTGAFile(filename, nullptr, image);
	ID3D11ShaderResourceView* pSRV = nullptr;
	if (SUCCEEDED(result))
	{
		result = CreateShaderResourceView(device, image.GetImages(),
			image.GetImageCount(), image.GetMetadata(), &pSRV);
		m_texture = pSRV;
		return true;
		if (FAILED(result))
		{
			return result;
		}
	}

	return result;
}

void TextureClass::Shutdown()
{
	// Release the texture resource.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;

}
