#include "cglTF_Model.hpp"
#include "../Camera.hpp"
#include <ImGui/imgui.h>

cglTF_Model::cglTF_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string_view Filepath)
	: cglTF_Importer(pDevice, pContext, Filepath)
{
	Create(pDevice, pContext, Filepath);
}

void cglTF_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string_view Filepath)
{
	if (bInitialized)
	{
		::OutputDebugStringA("[glTF] Model is already created!\n");
		return;
	}

	if (!Import(pDevice, pContext, Filepath))
	{
		throw std::exception();
	}

	m_VertexBuffer = std::make_unique<VertexBuffer<Vertex>>();
	m_VertexBuffer->Create(m_Device, m_Vertices.data(), m_Vertices.size());

	m_IndexBuffer = std::make_unique<IndexBuffer>();
	m_IndexBuffer->Create(m_Device, m_Indices.data(), m_Indices.size());

	m_ConstantBuffer = std::make_unique<ConstantBuffer<cbPerObject>>();
	m_ConstantBuffer->Create(m_Device, m_Context);

	m_cbMaterial = std::make_unique<ConstantBuffer<glTF_Material>>();
	m_cbMaterial->Create(m_Device, m_Context);

	if (!bHasAnimation)
	{
		m_Vertices.clear();
		m_Vertices.shrink_to_fit();
		m_Indices.clear();
		m_Indices.shrink_to_fit();
	}

	bInitialized = true;
}

void cglTF_Model::Draw(Camera* pCamera)
{
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	uint32_t offset{ 0 };
	m_Context->IASetVertexBuffers(0, 1, m_VertexBuffer->GetAddressOf(), m_VertexBuffer->GetStridePtr(), &offset);

	for (auto& node : m_Nodes)
	{
		DrawNode(pCamera, node);
	}
}

void cglTF_Model::DrawNode(Camera* pCamera, glTF::Node* pNode)
{
	if (pNode->Mesh)
	{
		UpdateWorld();
		m_ConstantBuffer->UpdateBuffer({ XMMatrixTranspose(pNode->Mesh->Matrix * m_WorldMatrix * pCamera->GetViewProjection()),											   XMMatrixTranspose(m_WorldMatrix) });
		m_Context->VSSetConstantBuffers(0, 1, m_ConstantBuffer->GetAddressOf());

		m_cbMaterial->UpdateBuffer({ *(XMFLOAT4*)m_Ambient.data(),
									 *(XMFLOAT3*)m_Diffuse.data(), 
									 *(XMFLOAT3*)m_Direction.data(),
									 *(XMFLOAT4*)m_Specular.data(),
									 m_SpecularIntensity });
									 
		m_Context->PSSetConstantBuffers(0, 1, m_cbMaterial->GetAddressOf());

		for (auto* primitive : pNode->Mesh->Primitives)
		{
			if (primitive->Material.BaseColorTexture)
				m_Context->PSSetShaderResources(0, 1, primitive->Material.BaseColorTexture->GetTextureViewAddressOf());
			if (primitive->Material.NormalTexture)
				m_Context->PSSetShaderResources(1, 1, primitive->Material.NormalTexture->GetTextureViewAddressOf());
			if (primitive->Material.MetallicTexture)
				m_Context->PSSetShaderResources(2, 1, primitive->Material.MetallicTexture->GetTextureViewAddressOf());
			if (primitive->Material.EmissiveTexture)
				m_Context->PSSetShaderResources(3, 1, primitive->Material.EmissiveTexture->GetTextureViewAddressOf());

			if (primitive->bHasIndices)
			{
				m_Context->IASetIndexBuffer(m_IndexBuffer->GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
				m_Context->DrawIndexed(primitive->IndexCount, primitive->FirstIndexLocation, primitive->BaseVertexLocation);
			}
			else
			{
				m_Context->Draw(primitive->VertexCount, primitive->StartVertexLocation);
			}
		}
	}

	for (auto child : pNode->Children)
		DrawNode(pCamera, child);

}

void cglTF_Model::DrawGUI()
{
	// Model transformation
	{
		ImGui::Text("Transforms");
		{
			if (ImGui::DragFloat3("Translation", m_Translations.data()))
			{
				m_Translation = XMVectorSet(m_Translations.at(0), 
											m_Translations.at(1), 
											m_Translations.at(2),
											0.0f);
			}

			if (ImGui::DragFloat3("Rotation", m_Rotations.data(), 0.5f, 0.0f, 360.0f))
			{
				m_Rotation = XMVectorSet(XMConvertToRadians(m_Rotations.at(0)),
										 XMConvertToRadians(m_Rotations.at(1)),
										 XMConvertToRadians(m_Rotations.at(2)),
										 0.0f);
			}

			if (ImGui::DragFloat3("Scale", m_Scales.data()))
			{
				m_Scale = XMVectorSet(m_Scales.at(0), 
									  m_Scales.at(1), 
									  m_Scales.at(2), 
									  0.0f);
			}
		}
	}
	// Shader data
	{
		ImGui::Text("Lights");
		ImGui::ColorEdit4("Ambient", m_Ambient.data());
		ImGui::ColorEdit3("Diffuse", m_Diffuse.data());
		ImGui::ColorEdit4("Specular", m_Specular.data());
		ImGui::DragFloat3("Direction", m_Direction.data());
		ImGui::DragFloat("SpecularIntensity", &m_SpecularIntensity);
	}

}

void cglTF_Model::UpdateWorld()
{
	m_WorldMatrix = XMMatrixIdentity();
	m_WorldMatrix = XMMatrixScalingFromVector(m_Scale) * XMMatrixRotationRollPitchYawFromVector(m_Rotation) * XMMatrixTranslationFromVector(m_Translation);
}
