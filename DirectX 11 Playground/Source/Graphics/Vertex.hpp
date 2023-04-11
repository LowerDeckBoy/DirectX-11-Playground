#pragma once
#include <DirectXMath.h>


struct SimpleVertex
{
	DirectX::XMFLOAT3 Position;
};

struct TexVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TexCoord;
};

struct TexNormalVertex
{
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TexCoord;
};

struct Color
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

struct ColorNormalTexture
{
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Tangent;
	DirectX::XMFLOAT3 Bitangent;
};
