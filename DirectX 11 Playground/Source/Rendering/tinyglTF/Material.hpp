#pragma once
#include "../../Graphics/Texture.hpp"
#include "Materials.hpp"

enum class eTextureSlots : uint32_t
{
	BaseColor = 0,
	NormalMap,
	MetallicRoughness,
	Emissive
};

struct Material
{
	Texture* BaseColorTexture { nullptr };
	Texture* NormalTexture{ nullptr };
	Texture* MetallicRoughnessTexture{ nullptr };
	Texture* EmissiveTexture{ nullptr };

	XMFLOAT4 BaseColorFactor{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };
	XMFLOAT4 EmissiveFactor { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };

	float MetallicFactor { 1.0f };
	float RoughnessFactor{ 1.0f };

	float AlphaCutoff{ 0.5f };
	bool bDoubleSided{ false };

	BOOL bHasDiffuse{ false };
	BOOL bHasNormal{ false };
	BOOL bHasMetallic{ false };
};

