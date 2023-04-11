#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_USE_CPP14
#include "ModelLoader.hpp"
#include "../Utils/Utils.hpp"
#include "../Utils/FileUtils.hpp"

bool ModelLoader::Load(const std::string& ModelPath)
{
	if (files::GetExtension(ModelPath) != ".gltf")
	{
		OutputDebugStringA("Imported Model is not of .gltf extension!\n");
		std::string msg = "Actual input file extension was: " + files::GetExtension(ModelPath) + '\n';
		OutputDebugStringA(msg.c_str());
		return false;
	}

	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string warning;
	std::string error;

	bool result = loader.LoadASCIIFromFile(&model, &error, &warning, ModelPath);

	if (!warning.empty())
	{
		::OutputDebugStringA(warning.c_str());
	}

	if (!error.empty())
	{
		::OutputDebugStringA(error.c_str());
	}

	if (!result)
	{
		::OutputDebugStringA("Model loading failed\n");
		return false;
	}

	this->Filepath = ModelPath;

	TraverseNode(model, 0);

	return true;
}

void ModelLoader::InitMeshes(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	for (auto& mesh : Meshes)
	{
		mesh.InitializeMesh(pDevice, pContext);
	}
}

void ModelLoader::InitMaterials(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	for (auto& material : Materials)
	{
		if (material.bDiffuse)
			material.TexDiffuse.LoadTexture(pDevice, pContext, material.TexDiffuse.GetPath());

		if (material.bNormal)
			material.TexNormal.LoadTexture(pDevice, pContext, material.TexNormal.GetPath());

		if (material.bMetallicRoughness)
			material.TexMetallicRoughness.LoadTexture(pDevice, pContext, material.TexMetallicRoughness.GetPath());

		if (material.bEmissive)
			material.TexEmissive.LoadTexture(pDevice, pContext, material.TexEmissive.GetPath());
	}
}

void ModelLoader::ProcessMesh(const tinygltf::Model& Model, const tinygltf::Mesh& gltfMesh, XMMATRIX& Matrix)
{
	for (const tinygltf::Primitive& primitive : gltfMesh.primitives)
	{
		std::vector<Vertex> vertices = ProcessVertices(Model, primitive);
		std::vector<uint32_t> indices = ProcessIndices(Model, primitive);
		ProcessMaterials(Model, primitive);
		Meshes.push_back(Mesh(vertices, indices, Matrix));
	}
}

std::vector<uint32_t> ModelLoader::ProcessIndices(const tinygltf::Model& Model, const tinygltf::Primitive& Primitive)
{
	std::vector<uint32_t> indices{};

	const tinygltf::Accessor& accessor = Model.accessors[Primitive.indices];
	const tinygltf::BufferView& bufferView = Model.bufferViews[accessor.bufferView];
	const tinygltf::Buffer& buffer = Model.buffers[bufferView.buffer];

	int stride = accessor.ByteStride(bufferView);
	indices.reserve(accessor.count);

	const unsigned char* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

	if (stride == 1)
	{
		for (size_t i = 0; i < accessor.count; i += 3)
		{
			indices.push_back(data[i + 0]);
			indices.push_back(data[i + 1]);
			indices.push_back(data[i + 2]);
		}
	}
	else if (stride == 2)
	{
		for (size_t i = 0; i < accessor.count; i += 3)
		{
			indices.push_back(((uint16_t*)data)[i + 0]);
			indices.push_back(((uint16_t*)data)[i + 1]);
			indices.push_back(((uint16_t*)data)[i + 2]);
		}
	}
	else if (stride == 4)
	{
		for (size_t i = 0; i < accessor.count; i += 3)
		{
			indices.push_back(((uint32_t*)data)[i + 0]);
			indices.push_back(((uint32_t*)data)[i + 1]);
			indices.push_back(((uint32_t*)data)[i + 2]);
		}
	}
	else
	{
		OutputDebugStringA("Invalid stride offset!\n");
		return std::vector<uint32_t>();
	}

	return indices;
}

std::vector<Vertex> ModelLoader::ProcessVertices(const tinygltf::Model& Model, const tinygltf::Primitive& Primitive)
{
	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT2> uvs;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT3> tangents;
	
	std::vector<float> tangents_ws;
	std::vector<DirectX::XMFLOAT3> bitangents;

	std::vector<Vertex> vertices{};

	for (const auto& attrib : Primitive.attributes)
	{
		std::string attribName = attrib.first;
		int attribData = attrib.second;

		const tinygltf::Accessor& accessor = Model.accessors[attribData];
		const tinygltf::BufferView& bufferView = Model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = Model.buffers[bufferView.buffer];

		int stride = accessor.ByteStride(bufferView);
		size_t vertexCount = accessor.count;
		vertices.reserve(vertexCount);

		const unsigned char* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;


		if (!attribName.compare("POSITION"))
		{
			positions.reserve(vertexCount);
			for (size_t i = 0; i < vertexCount; ++i)
				positions.push_back(*(DirectX::XMFLOAT3*)((size_t)data + i * stride));
		}
		else if (!attribName.compare("NORMAL"))
		{
			normals.reserve(vertexCount);
			for (size_t i = 0; i < vertexCount; ++i)
				normals.push_back(*(DirectX::XMFLOAT3*)((size_t)data + i * stride));
		}
		else if (!attribName.compare("TANGENT"))
		{
			tangents.reserve(vertexCount);
			for (size_t i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT4 tangent = *(DirectX::XMFLOAT4*)((size_t)data + i * stride);
				tangents.push_back({ tangent.x, tangent.y, tangent.z });

				tangents_ws.push_back(tangent.w);
			}
		}
		else if (!attribName.compare("TEXCOORD_0"))
		{
			uvs.reserve(vertexCount);

			if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
			{
				for (size_t i = 0; i < vertexCount; ++i)
				{
					DirectX::XMFLOAT2 tex = *(DirectX::XMFLOAT2*)((size_t)data + i * stride);
					//tex.y = 1.0f - tex.y; // -> mostly breaks UVs
					uvs.push_back(tex);
				}
			}
			else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
			{
				for (size_t i = 0; i < vertexCount; ++i)
				{
					auto const& s = *(UINT8*)((size_t)data + i * stride + 0 * sizeof(UINT8));
					auto const& t = *(UINT8*)((size_t)data + i * stride + 1 * sizeof(UINT8));
					uvs.push_back(DirectX::XMFLOAT2(s / 255.0f, 1.0f - t / 255.0f));
				}
			}
			else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			{
				for (size_t i = 0; i < vertexCount; ++i)
				{
					uint16_t const& s = *(UINT16*)((size_t)data + i * stride + 0 * sizeof(UINT16));
					uint16_t const& t = *(UINT16*)((size_t)data + i * stride + 1 * sizeof(UINT16));
					uvs.push_back(DirectX::XMFLOAT2(s / 65535.0f, 1.0f - t / 65535.0f));
				}
			}
		}
	}

	size_t count = positions.size();
	if (uvs.size() != count)
		uvs.resize(count);
	if (normals.size() != count)
		normals.resize(count);
	if (tangents.size() != count)
		tangents.resize(count);

	if (tangents_ws.size() != count)
		tangents_ws.resize(count);
	if (bitangents.size() != count)
		bitangents.resize(count);


	vertices.reserve(vertices.size() + count);
	for (size_t i = 0; i < count; i++)
	{
		XMVECTOR _bitangent = XMVectorScale(XMVector3Cross(XMLoadFloat3(&normals.at(i)), XMLoadFloat3(&tangents.at(i))), tangents_ws.at(i));
		XMStoreFloat3(&bitangents[i], XMVector3Normalize(_bitangent));

		vertices.push_back({ positions.at(i), uvs.at(i), normals.at(i), tangents.at(i), bitangents.at(i) });
	}
	
	return vertices;
}

void ModelLoader::ProcessMaterials(const tinygltf::Model& Model, const tinygltf::Primitive& Primitive)
{
	const tinygltf::Material& material = Model.materials[Primitive.material];
	const tinygltf::PbrMetallicRoughness& pbrMaterial = material.pbrMetallicRoughness;

	TMaterial newMaterial;

	if (pbrMaterial.baseColorTexture.index >= 0)
	{
		const tinygltf::Texture& texture = Model.textures[pbrMaterial.baseColorTexture.index];
		const tinygltf::Image& image = Model.images[texture.source];

		std::string texPath = files::glTF::GetTexturePath(Filepath, image.uri);
		newMaterial.m_pbrMaterial.BaseColorFactor.x = static_cast<float>(pbrMaterial.baseColorFactor[0]);
		newMaterial.m_pbrMaterial.BaseColorFactor.y = static_cast<float>(pbrMaterial.baseColorFactor[1]);
		newMaterial.m_pbrMaterial.BaseColorFactor.z = static_cast<float>(pbrMaterial.baseColorFactor[2]);
		newMaterial.m_pbrMaterial.BaseColorFactor.w = static_cast<float>(pbrMaterial.baseColorFactor[3]);

		newMaterial.TexDiffuse.Init(texPath, image.width, image.height, ETextureType::BaseColor);
		newMaterial.bDiffuse = true;
	}

	if (material.normalTexture.index >= 0)
	{
		const tinygltf::Texture& texture = Model.textures[material.normalTexture.index];
		const tinygltf::Image& image = Model.images[texture.source];

		std::string texPath = files::glTF::GetTexturePath(Filepath, image.uri);

		newMaterial.TexNormal.Init(texPath, image.width, image.height, ETextureType::NormalMap);
		newMaterial.bNormal = true;
	}

	if (pbrMaterial.metallicRoughnessTexture.index >= 0)
	{
		const tinygltf::Texture& texture = Model.textures[pbrMaterial.metallicRoughnessTexture.index];
		const tinygltf::Image& image = Model.images[texture.source];

		std::string texPath = files::glTF::GetTexturePath(Filepath, image.uri);

		newMaterial.m_pbrMaterial.MetallicFactor = static_cast<float>(pbrMaterial.metallicFactor);
		newMaterial.m_pbrMaterial.RoughnessFactor = static_cast<float>(pbrMaterial.roughnessFactor);
				  
		newMaterial.TexMetallicRoughness.Init(texPath, image.width, image.height, ETextureType::MetallicRoughness);
		newMaterial.bMetallicRoughness = true;
	}

	if (material.emissiveTexture.index >= 0)
	{
		const tinygltf::Texture& texture = Model.textures[material.emissiveTexture.index];
		const tinygltf::Image& image = Model.images[texture.source];

		std::string texPath = files::glTF::GetTexturePath(Filepath, image.uri);

		newMaterial.m_pbrMaterial.EmissiveFactor.x = static_cast<float>(material.emissiveFactor.at(0));
		newMaterial.m_pbrMaterial.EmissiveFactor.y = static_cast<float>(material.emissiveFactor.at(1));
		newMaterial.m_pbrMaterial.EmissiveFactor.z = static_cast<float>(material.emissiveFactor.at(2));

		//material.ap

		newMaterial.TexEmissive.Init(texPath, image.width, image.height, ETextureType::EmissiveTexture);
		newMaterial.bEmissive = true;
	}

	Materials.push_back(newMaterial);
}

void ModelLoader::TraverseNode(const tinygltf::Model& Model, uint32_t NextNode, XMMATRIX Matrix)
{
	const tinygltf::Node& node = Model.nodes[NextNode];

	Transforms _tranforms{};

	if (!node.translation.empty())
	{
		_tranforms.Translation = *(XMFLOAT3*)node.translation.data();
	}

	if (!node.rotation.empty())
	{
		_tranforms.Rotation = *(XMFLOAT4*)node.rotation.data();
	}

	if (!node.scale.empty())
	{
		_tranforms.Scale = *(XMFLOAT3*)node.scale.data();
	}

	if (!node.matrix.empty())
	{
		_tranforms.Matrix = *(XMMATRIX*)node.matrix.data();
	}
	
	XMVECTOR translation = XMLoadFloat3(&_tranforms.Translation);
	XMVECTOR rotation = XMLoadFloat4(&_tranforms.Rotation);
	XMVECTOR scale = XMLoadFloat3(&_tranforms.Scale);
	
	XMMATRIX world = _tranforms.Matrix * XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rotation) * XMMatrixTranslationFromVector(translation);

	XMMATRIX nextMatrix = world * Matrix;

	if (node.mesh >= 0)
	{
		ProcessMesh(Model, Model.meshes[node.mesh], nextMatrix);
	}

	if (!node.children.empty())
	{
		for (const auto& child : node.children)
		{
			TraverseNode(Model, child, nextMatrix);
		}
	}
}
