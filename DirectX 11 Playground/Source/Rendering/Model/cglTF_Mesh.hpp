#pragma once
#include "../../Graphics/Vertex.hpp"
#include "../../Graphics/Texture.hpp"
#include <vector>

#include <DirectXMath.h>
using namespace DirectX;

namespace glTF
{ 
	struct Material
	{
		std::string_view Name;

		XMFLOAT4 BaseColorFactor { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };
		XMFLOAT3 EmissiveFactor  { XMFLOAT3(1.0f, 1.0f, 1.0f) };

		float MetallicFactor	 { 1.0f };
		float RoughnessFactor	 { 1.0f };
		float AlphaCutoff		 { 0.5f };
		
		bool bDoubleSided{ false };

		Texture* BaseColorTexture{ nullptr };
		Texture* NormalTexture	 { nullptr };
		Texture* MetallicTexture { nullptr };
		Texture* EmissiveTexture { nullptr };
	};

	struct Primitive 
	{
		uint32_t FirstIndexLocation{ 0 };
		uint32_t BaseVertexLocation{ 0 };

		uint32_t IndexCount { 0 };
		uint32_t VertexCount{ 0 };
		uint32_t StartVertexLocation{ 0 };
		bool bHasIndices{ false };

		Material Material;
	};

	struct Mesh
	{
		Mesh() = default;
		std::string Name;
		XMMATRIX Matrix{ XMMatrixIdentity() };
		std::vector<Primitive*> Primitives;
	};

	struct Node
	{
		Node* Parent;
		std::vector<Node*> Children;
		std::string Name{ "None Given" };

		Mesh* Mesh{ nullptr };

		XMMATRIX Matrix		{ XMMatrixIdentity() };
		XMMATRIX LocalMatrix{ XMMatrixIdentity() };
		XMFLOAT3 Translation{ XMFLOAT3(0.0f, 0.0f, 0.0f) };
		XMFLOAT4 Rotation	{ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
		XMFLOAT3 Scale		{ XMFLOAT3(1.0f, 1.0f, 1.0f) };

	};
}
