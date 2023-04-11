#pragma once

#include "tinygltf/tiny_gltf.h"
#include <vector>
#include <wrl.h>
#include "../../Graphics/Buffers/VertexBuffer.hpp"
#include "../../Graphics/Buffers/IndexBuffer.hpp"
#include "../../Graphics/Buffers/ConstantBuffer.hpp"

#include <DirectXMath.h>
using namespace DirectX;

struct Mesh
{
public:
	Mesh();
	Mesh(const Mesh& rhs);
	Mesh(std::vector<Vertex>& Vertices, std::vector<uint32_t>& Indices, XMMATRIX& Matrix);
	~Mesh() {}

	void InitializeMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void Draw();

	VertexBuffer<Vertex> m_VertexBuffer;
	IndexBuffer m_IndexBuffer;

	XMMATRIX Matrix = XMMatrixIdentity();


private:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;

public:
	std::vector<Vertex> m_Vertices{};
	std::vector<uint32_t> m_Indices{};

};

