#pragma once
#include "cglTF_Importer.hpp"
#include "../../Graphics/Vertex.hpp"
#include "../../Graphics/VertexBuffer.hpp"
#include "../../Graphics/IndexBuffer.hpp"
#include "../../Graphics/ConstantBuffer.hpp"
#include <array>
#include <memory>

class Camera;

class cglTF_Model : public cglTF_Importer
{
public:
	cglTF_Model() {}
	cglTF_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string_view Filepath);
	~cglTF_Model() = default;

	void Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string_view Filepath);
	void Draw(Camera* pCamera);
	void DrawNode(Camera* pCamera, glTF::Node* pNode);
	
	void DrawGUI();

private:
	std::unique_ptr<VertexBuffer<Vertex>> m_VertexBuffer{ nullptr };
	std::unique_ptr<IndexBuffer> m_IndexBuffer{ nullptr };

	std::unique_ptr<ConstantBuffer<cbPerObject>> m_ConstantBuffer{ nullptr };
	std::unique_ptr<ConstantBuffer<glTF_Material>> m_cbMaterial{ nullptr };

	bool bHasAnimation{ false };

private:
	// World transforms
	void UpdateWorld();
	XMMATRIX m_WorldMatrix{ XMMatrixIdentity() };
	XMVECTOR m_Translation{ XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f) };
	XMVECTOR m_Rotation{ XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
	XMVECTOR m_Scale{ XMVectorSet(1.0f, 1.0f, 1.0f, 0.0) };
	// For GUI usage
	std::array<float, 3> m_Translations	{ 0.0f, 0.0f, 0.0f };
	std::array<float, 3> m_Rotations	{ 0.0f, 0.0f, 0.0f };
	std::array<float, 3> m_Scales		{ 1.0f, 1.0f, 1.0f };

	// Shader data
	std::array<float, 4> m_Ambient	{ 1.0f, 1.0f, 1.0f, 1.0f };
	std::array<float, 3> m_Diffuse	{ 1.0f, 1.0f, 1.0f };
	std::array<float, 4> m_Specular	{ 1.0f, 1.0f, 1.0f, 1.0f };
	float m_SpecularIntensity		{ 32.0f };
	std::array<float, 3> m_Direction{ 1.0f, 1.0f, 1.0f };

	bool bInitialized{ false };
};

