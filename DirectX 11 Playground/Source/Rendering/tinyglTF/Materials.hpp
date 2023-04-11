#pragma once
#include <DirectXMath.h>

using namespace DirectX;
struct cbNormalMapping
{
	XMFLOAT4 Ambient;
	alignas(16) XMFLOAT3 Diffuse;
	alignas(16) XMFLOAT3 Directions;
	BOOL bHasDiffuse;
	BOOL bHasNormals;
	BOOL bHasMetallic;
	float Intensity;
};

struct cbPBRMaterial
{
	XMFLOAT4 Direction;
	XMFLOAT4 CamPos;
	XMFLOAT4 Diffuse;
	// Ambient
	XMFLOAT4 BaseColorFactor;

	float MetallicFactor;
	float RoughnessFactor;
	XMFLOAT4 EmissiveFactor;

	float AlphaCutoff;
	bool bDoubleSided;

	BOOL bHasDiffuse;
	BOOL bHasNormal;
	BOOL bHasMetallic;
};

struct cbSpotLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	float Intensity;
	float Range;
	float ConeAngle;
	float Padding;

	float Attenuation_A;
	float Attenuation_B;
	float Attenuation_C;
	float Padding2;

	XMFLOAT4 Position;
	XMFLOAT4 Direction;

	bool bHasNormals;
	void InitData()
	{
		Ambient = XMFLOAT4(0.35f, 0.35f, 0.35f, 1.0f);
		Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		Intensity = 1.0f;
		Range = 500.0f;
		ConeAngle = 45.0f;
		Padding = 0.0f;
		Attenuation_A = 0.4f;
		Attenuation_B = 0.004f;
		Attenuation_C = 0.0f;
		Padding2 = 0.0f;
		Position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		Direction = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	}
};


//TEST
struct cbCameraBuffer
{
	alignas(16) XMFLOAT3 CameraPosition;
};

//TEST
struct cbSpecularLight
{
	XMFLOAT4 Ambient;
	alignas(16) XMFLOAT3 Diffuse;
	alignas(16) XMFLOAT3 Direction;
	XMFLOAT4 Specular;
	alignas(16) float SpecularIntensity;
};