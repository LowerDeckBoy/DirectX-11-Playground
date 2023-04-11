#include "Model.hpp"
#include "../Utils/Utils.hpp"
#include "ImGui/imgui.h"
#include "Camera.hpp"


Model::Model() :
	m_WorldMatrix(XMMatrixIdentity()),
	m_RotationMatrix(XMMATRIX()),
	m_Ambient({ 1.0f, 1.0f, 1.0f, 1.0f}),
	m_Diffuse({1.0f, 1.0f, 1.0f}),
	m_Distances({0.0f, 0.0f, 0.0f}),
	m_BlendAll(1.0f),
	m_Translation(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
	m_Rotation(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
	m_Scale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f)),
	m_Translations({0.0f, 0.0f, 0.0f}),
	m_Rotations({0.0f, 0.0f, 0.0f}),
	m_Scales({1.0f, 1.0f, 1.0f})
{

}

Model::Model(const std::string& Filepath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_Device = pDevice;
	m_Context = pContext;

	m_Loader.Load(Filepath);
	ThrowIfFailed(m_ConstantBuffer.Create(pDevice, pContext));

	m_WorldMatrix = XMMatrixIdentity();

	UpdateWorld();
}

Model::Model(const std::string& Filepath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Camera& Cam)
{
	m_Device = pDevice;
	m_Context = pContext;

	m_Loader.Load(Filepath);
	ThrowIfFailed(m_ConstantBuffer.Create(pDevice, pContext));

	m_WorldMatrix = XMMatrixIdentity();

	m_Ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_Diffuse = { 1.0f, 1.0f, 1.0f,};
	m_Distances = { 0.0f, 0.0f, 0.0f };
	m_BlendAll = 1.0f;
	
	UpdateWorld();
}

Model::~Model()
{

}

void Model::Init(const std::string& Filepath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_Device = pDevice;
	m_Context = pContext;

	m_Loader.Load(Filepath);
	m_Loader.InitMeshes(pDevice, pContext);

	ThrowIfFailed(m_ConstantBuffer.Create(pDevice, pContext));
	ThrowIfFailed(m_cbMaterial.Create(pDevice, pContext));

	m_WorldMatrix = XMMatrixIdentity();

	m_Ambient   = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_Diffuse   = { 1.0f, 1.0f, 1.0f };
	m_Distances = { 1.0f, -1.0f, 1.0f };


}

void Model::Init(const std::string& Filepath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string ModelName)
{
	if (pDevice == nullptr || pContext == nullptr)
	{
		OutputDebugStringA("Model Class: pDevice or pContext is nullptr!\n");
		throw std::exception().what();
	}

	m_Device = pDevice;
	m_Context = pContext;

	m_Loader.Load(Filepath);
	m_Loader.InitMeshes(pDevice, pContext);
	m_Loader.InitMaterials(pDevice, pContext);

	ThrowIfFailed(m_ConstantBuffer.Create(pDevice, pContext));
	ThrowIfFailed(m_cbMaterial.Create(pDevice, pContext));

	m_WorldMatrix = XMMatrixIdentity();

	m_Ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_Diffuse = { 1.0f, 1.0f, 1.0f };
	m_Distances = { 0.0f, 0.0f, 1.0f };

	m_ModelName = ModelName;

	// TEST 
	ThrowIfFailed(m_cbPBR.Create(pDevice, pContext));

	// TEST
	ThrowIfFailed(m_cbSpecular.Create(pDevice, pContext));
	ThrowIfFailed(m_cbCamera.Create(pDevice, pContext));


	UpdateWorld();
}

float Model::Intensity = 1.0f;
void Model::Draw(Camera& Cam)
{
	for (int i = 0; i < m_Loader.Meshes.size(); i++)
	{
		UpdateWorld();
		//auto matrix = m_Loader.Meshes.at(i).Matrix;

		m_ConstantBuffer.UpdateBuffer({ XMMatrixTranspose(m_Loader.Meshes.at(i).Matrix * m_WorldMatrix * Cam.GetViewProjection()), XMMatrixTranspose(m_WorldMatrix * XMMatrixIdentity()) });
		//m_ConstantBuffer.UpdateBuffer({ XMMatrixTranspose(matrix * m_WorldMatrix * Cam.GetViewProjection()), XMMatrixTranspose(m_WorldMatrix * XMMatrixIdentity()) });
		m_Context.Get()->VSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());

		m_cbCamera.UpdateBuffer({ XMFLOAT3(Cam.GetPosition().m128_f32[0], Cam.GetPosition().m128_f32[1] ,Cam.GetPosition().m128_f32[2]) });
		m_Context.Get()->VSSetConstantBuffers(1, 1, m_cbCamera.GetAddressOf());

		m_cbSpecular.UpdateBuffer({
					XMFLOAT4(m_Ambient.at(0), m_Ambient.at(1), m_Ambient.at(2), m_Ambient.at(3)),
					XMFLOAT3(m_Diffuse.at(0), m_Diffuse.at(1), m_Diffuse.at(2)),
					XMFLOAT3(m_Distances.at(0), -m_Distances.at(1), m_Distances.at(2)),
					XMFLOAT4(m_Specular.at(0), m_Specular.at(1), m_Specular.at(2), m_Specular.at(3)),
					SpecularIntensity
								  });

		m_Context.Get()->PSSetConstantBuffers(0, 1, m_cbSpecular.GetAddressOf());

		if (m_Loader.Materials.at(i).bDiffuse)
			m_Context.Get()->PSSetShaderResources(0, 1, m_Loader.Materials.at(i).TexDiffuse.GetTextureViewAddressOf());

		if (m_Loader.Materials.at(i).bNormal)
			m_Context.Get()->PSSetShaderResources(1, 1, m_Loader.Materials.at(i).TexNormal.GetTextureViewAddressOf());

		if (m_Loader.Materials.at(i).bMetallicRoughness)
			m_Context.Get()->PSSetShaderResources(2, 1, m_Loader.Materials.at(i).TexMetallicRoughness.GetTextureViewAddressOf());

		if (m_Loader.Materials.at(i).bEmissive)
			m_Context.Get()->PSSetShaderResources(3, 1, m_Loader.Materials.at(i).TexEmissive.GetTextureViewAddressOf());

		m_Loader.Meshes[i].Draw();
	}
}

void Model::UpdateWorld()
{
	m_WorldMatrix = XMMatrixIdentity();

	RotateY();

	m_WorldMatrix =  XMMatrixScalingFromVector(m_Scale) * XMMatrixRotationRollPitchYawFromVector(m_Rotation) * XMMatrixTranslationFromVector(m_Translation);

}

float Model::rotY = 0.0f;
void Model::RotateY()
{
	if (bShouldRotateY)
	{
		rotY += 0.015f;
		if (rotY >= 360.0f)
			rotY = 0.0f;

		m_Rotations.at(1) = rotY;
		m_Rotation.m128_f32[1] = XMConvertToRadians(rotY);
	}
}

void Model::DrawGUI()
{
	ImGui::ColorEdit4("Ambient", m_Ambient.data());
	ImGui::SliderFloat("Alpha", &m_Ambient.at(3), 0.0f, 1.0f);
	ImGui::ColorEdit3("Diffuse", m_Diffuse.data());

	ImGui::SliderFloat("Metallic", &Metallic, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &Roughness, 0.0f, 1.0f);

	if (ImGui::SliderFloat("Blend", &m_BlendAll, 0.0f, 1.0f))
		m_Ambient = { m_BlendAll, m_BlendAll, m_BlendAll, m_BlendAll };

	//if (ImGui::DragFloat3("Distances", m_Distances.data(), 0.25f))
	//{
	//	m_Distances
	//}

	ImGui::Text("Transforms");
	if (ImGui::DragFloat3("Translations", m_Translations.data(), 0.25f))
	{
		m_Translation.m128_f32[0] = m_Translations.at(0);
		m_Translation.m128_f32[1] = m_Translations.at(1);
		m_Translation.m128_f32[2] = m_Translations.at(2);
		m_Translation.m128_f32[3] = 0.0f;
	}
	
	if (ImGui::DragFloat3("Rotations", m_Rotations.data(), 0.25f, -360.0f, 360.0f))
	{
		m_Rotation.m128_f32[0] = XMConvertToRadians(m_Rotations.at(0));
		m_Rotation.m128_f32[1] = XMConvertToRadians(m_Rotations.at(1));
		m_Rotation.m128_f32[2] = XMConvertToRadians(m_Rotations.at(2));
		m_Rotation.m128_f32[3] = 0.0f;
	}

	if (ImGui::DragFloat3("Scales", m_Scales.data(), 0.25, 0.0f, 100.0f))
	{
		m_Scale.m128_f32[0] = m_Scales.at(0);
		m_Scale.m128_f32[1] = m_Scales.at(1);
		m_Scale.m128_f32[2] = m_Scales.at(2);
		m_Scale.m128_f32[3] = 0.0f;
	}

	if (ImGui::Checkbox("Rotate Around", &bShouldRotateY))
		bShouldRotateY ^= false;

	ImGui::ColorEdit4("Specular Color", m_Specular.data());
	ImGui::SliderFloat("Specular Intensity", &SpecularIntensity, 0.0f, 100.0f);
}
