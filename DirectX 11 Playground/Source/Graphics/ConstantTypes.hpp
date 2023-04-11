#pragma once
#include <DirectXMath.h>

struct cbPerObject
{
	XMMATRIX WVP;
	XMMATRIX World;
};

struct glTF_Material
{
	XMFLOAT4 Ambient{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };
	alignas(16) XMFLOAT3 Diffuse{ XMFLOAT3(1.0f, 1.0f, 1.0f) };
	alignas(16) XMFLOAT3 Direction{ XMFLOAT3(0.0f, 0.0f, 1.0f) };
	DirectX::XMFLOAT4 Specular{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };
	float SpecularIntensity;
};

struct cbLights
{
	XMFLOAT4 DiffuseColor;
	XMFLOAT3 LightDirection;
	//float padding;
	BOOL bHasNormals;
};