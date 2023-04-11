#pragma once
#include "cglTF_Mesh.hpp"

struct cgltf_data;
struct cgltf_node;
struct cgltf_mesh;
struct cgltf_primitive;

class cglTF_Importer
{
public:
	cglTF_Importer() {}
	cglTF_Importer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string_view Filepath);
	bool Import(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string_view Filepath);

	void ProcessNode(cgltf_data* pData, cgltf_node* pNode, glTF::Node* ParentNode, XMMATRIX ParentMatrix);
	glTF::Mesh* ProcessMesh(cgltf_data* pData, cgltf_mesh* pMesh, XMMATRIX Matrix);
	glTF::Material* ProcessMaterial(cgltf_data* pData, cgltf_primitive* pPrimitive);

protected:
	ID3D11Device* m_Device{ nullptr };
	ID3D11DeviceContext* m_Context{ nullptr };

	std::string_view m_ModelPath;

	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	std::vector<glTF::Node*> m_Nodes;

	bool bLoaded{ false };
};
