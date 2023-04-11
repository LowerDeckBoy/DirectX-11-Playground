#pragma once
#define STBI_MSC_SECURE_CRT // -> for deprecated methods
//#define _CRT_SECURE_NO_WARNINGS

#include "Mesh.hpp"
#include "Material.hpp"


class ModelLoader
{
public:
	bool Load(const std::string& ModelPath);
	void InitMeshes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void InitMaterials(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void ProcessMesh(const tinygltf::Model& Model, const tinygltf::Mesh& gltfMesh, XMMATRIX& Matrix);

	std::vector<uint32_t> ProcessIndices(const tinygltf::Model& Model, const tinygltf::Primitive& Primitive);
	std::vector<Vertex> ProcessVertices(const tinygltf::Model& Model, const tinygltf::Primitive& Primitive);
	void ProcessMaterials(const tinygltf::Model& Model, const tinygltf::Primitive& Primitive);

	void TraverseNode(const tinygltf::Model& Model, uint32_t NextNode, XMMATRIX Matrix = XMMatrixIdentity());

	std::string Filepath;
	std::vector<Mesh> Meshes;
	std::vector<TMaterial> Materials;

};
