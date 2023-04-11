#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <XTK/WICTextureLoader11.h>
#include <DirectXTex.h>
#include <string>

using Microsoft::WRL::ComPtr;

class Texture
{
public:
	Texture() = default;
	Texture(const Texture& rhs);
	Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string_view& TexturePath);

	// With mipmapping
	void LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string_view& TexturePath);
	// None HDR/Cubemap texture loading
	void Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string_view& TexturePath);
	// TODO: To finish later
	void CreateFromHDR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string_view& TexturePath);

	ID3D11Resource* GetTexture() const { return m_Texture.Get(); }
	ID3D11ShaderResourceView* GetTextureView() const { return m_TextureView.Get(); }
	ID3D11ShaderResourceView** GetTextureViewAddressOf() { return m_TextureView.GetAddressOf(); }

	std::string GetPath() const { return m_TexturePath; }

private:
	ComPtr<ID3D11Resource> m_Texture;
	ComPtr<ID3D11ShaderResourceView> m_TextureView;

	std::string m_TexturePath;

	uint32_t m_Width{ 0 };
	uint32_t m_Height{ 0 };

};

