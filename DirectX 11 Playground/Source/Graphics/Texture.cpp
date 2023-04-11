#include "Texture.hpp"
#include "../Utils/Utils.hpp"
#include "../Utils/FileUtils.hpp"
//#include <DirectXTex.h>

Texture::Texture(const Texture& rhs)
{
	this->m_Texture = rhs.m_Texture;
	this->m_TextureView = rhs.m_TextureView;
	this->m_TexturePath = rhs.m_TexturePath;
	this->m_Width = rhs.m_Width;
	this->m_Height = rhs.m_Height;
}

Texture::Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string_view& TexturePath)
{
	
	//Create(pDevice, pContext, TexturePath);
	// TEST
	std::wstring widestr = std::wstring(TexturePath.begin(), TexturePath.end());
	const wchar_t* path = widestr.c_str();

	DirectX::ScratchImage scratchImage{};
	DirectX::LoadFromWICFile(path, DirectX::WIC_FLAGS_FORCE_RGB, nullptr, scratchImage);

	DirectX::TexMetadata metadata{ scratchImage.GetMetadata() };
	if (files::GetExtension(TexturePath.data()) == ".hdr")
	{
		CreateFromHDR(pDevice, pContext, TexturePath);
	}
	else
	{
		LoadTexture(pDevice, pContext, TexturePath);	
	}

}

void Texture::LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string_view& TexturePath)
{
	assert(pDevice != nullptr);
	assert(pContext != nullptr);

	std::wstring widestr = std::wstring(TexturePath.begin(), TexturePath.end());
	const wchar_t* path = widestr.c_str();

	// Mipmaps texture 
	ThrowIfFailed(DirectX::CreateWICTextureFromFileEx(
		pDevice, pContext,
		path, 0, 
		D3D11_USAGE_DEFAULT, (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET), 
		0,
		D3D11_RESOURCE_MISC_GENERATE_MIPS, 
		DirectX::WIC_LOADER_DEFAULT,
		nullptr, m_TextureView.GetAddressOf()));

	pContext->GenerateMips(m_TextureView.Get());
}

void Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string_view& TexturePath)
{
	std::wstring widestr = std::wstring(TexturePath.begin(), TexturePath.end());
	const wchar_t* path = widestr.c_str();

	DirectX::ScratchImage scratchImage{};
	DirectX::LoadFromWICFile(path, DirectX::WIC_FLAGS_NONE, nullptr, scratchImage);

	DirectX::TexMetadata metadata{ scratchImage.GetMetadata() };

	D3D11_TEXTURE2D_DESC desc{};
	desc.Format = metadata.format;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Width = static_cast<uint32_t>(metadata.width);
	desc.Height = static_cast<uint32_t>(metadata.height);
	desc.ArraySize = static_cast<uint32_t>(metadata.arraySize);
	desc.MipLevels = static_cast<uint32_t>(metadata.mipLevels);
	desc.CPUAccessFlags = 0;
	desc.BindFlags = (D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA subresource{};
	subresource.pSysMem			 = scratchImage.GetImages()->pixels;
	subresource.SysMemPitch		 = static_cast<uint32_t>(scratchImage.GetImages()->rowPitch);
	subresource.SysMemSlicePitch = static_cast<uint32_t>(scratchImage.GetImages()->slicePitch);
	
	ID3D11Texture2D* tex;
	ThrowIfFailed(pDevice->CreateTexture2D(&desc, &subresource, &tex));

	m_Texture = static_cast<ID3D11Texture2D*>(tex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = desc.Format;
	if (metadata.dimension == DirectX::TEX_DIMENSION_TEXTURE1D)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		srvDesc.Texture1D.MipLevels = desc.MipLevels;
		srvDesc.Texture1D.MostDetailedMip = 0;
	}
	else if (metadata.dimension == DirectX::TEX_DIMENSION_TEXTURE2D)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
	}
	else if (metadata.dimension == DirectX::TEX_DIMENSION_TEXTURE3D)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MipLevels = desc.MipLevels;
		srvDesc.Texture3D.MostDetailedMip = 0;
	}

	ThrowIfFailed(pDevice->CreateShaderResourceView(m_Texture.Get(), &srvDesc, m_TextureView.GetAddressOf()));
	pContext->GenerateMips(m_TextureView.Get());
}

void Texture::CreateFromHDR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string_view& TexturePath)
{
	::OutputDebugStringA("Texture is HDR map.\n");

	std::wstring widestr = std::wstring(TexturePath.begin(), TexturePath.end());
	const wchar_t* path = widestr.c_str();
	DirectX::ScratchImage scratchImage{};
	DirectX::LoadFromHDRFile(path, nullptr, scratchImage);

	DirectX::TexMetadata metadata{ scratchImage.GetMetadata() };

}
