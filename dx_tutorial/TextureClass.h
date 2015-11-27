#pragma once

#include <d3d11.h>
#include "DDSTextureLoader\DDSTextureLoader.h"

using namespace DirectX;

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device* device, WCHAR* filename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* texture;
};

