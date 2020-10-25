#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <d3d11.h>
#include <DirectXTex.h>
#include <DirectXTex.inl>

using namespace DirectX;
using namespace std;

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();
	bool Initialize(ID3D11Device*, LPCWSTR);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};

#endif