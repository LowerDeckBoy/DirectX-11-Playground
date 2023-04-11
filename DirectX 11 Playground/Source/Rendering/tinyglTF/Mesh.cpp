#include "Mesh.hpp"
#include "../Utils/Utils.hpp"

Mesh::Mesh(const Mesh& rhs)
{
	this->Matrix = rhs.Matrix;
	//this->m_Translation = rhs.m_Translation;
	//this->m_Rotation = rhs.m_Rotation;
	//this->m_Scale = rhs.m_Scale;
	//this->World = rhs.World;

	this->m_Vertices = rhs.m_Vertices;
	this->m_Indices = rhs.m_Indices;

	this->m_Context = rhs.m_Context;
}

Mesh::Mesh()
{
	m_Vertices = {};
	m_Indices = {};
	//m_Textures = {};
}

Mesh::Mesh(std::vector<Vertex>& Vertices, std::vector<uint32_t>& Indices, XMMATRIX& Matrix) :
	m_Vertices(Vertices),
	m_Indices(Indices),
	Matrix(Matrix)
{

}

void Mesh::InitializeMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (pContext == nullptr || pDevice == nullptr)
	{
		OutputDebugStringA("Mesh Class: pDevice or pContext are nullptr!\n");
		throw std::exception().what();
	}

	m_Context = pContext;

	ThrowIfFailed(m_VertexBuffer.Create(pDevice, m_Vertices.data(), static_cast<uint32_t>(m_Vertices.size())));
	ThrowIfFailed(m_IndexBuffer.Create(pDevice, m_Indices.data(), static_cast<uint32_t>(m_Indices.size())));

	m_Vertices.clear();
	m_Vertices.shrink_to_fit();
	m_Indices.clear();
	m_Indices.shrink_to_fit();
}

void Mesh::Draw()
{
	UINT offset = 0;
	m_Context.Get()->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), m_VertexBuffer.GetStridePtr(), &offset);
	m_Context.Get()->IASetIndexBuffer(m_IndexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
	m_Context.Get()->DrawIndexed(m_IndexBuffer.GetBufferSize(), 0, 0);
}
