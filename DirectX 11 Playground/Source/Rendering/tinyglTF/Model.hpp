#pragma once
#include "ModelLoader.hpp"
#include "Camera.hpp"


class Model
{
public:
	Model();
	Model(const std::string& Filepath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Model(const std::string& Filepath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Camera& Cam);
	~Model();
	
	void Init(const std::string& Filepath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Init(const std::string& Filepath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string ModelName);

	void Draw(Camera& Cam);

	void DrawGUI();

	ModelLoader GetLoader() const { return m_Loader; }

private:
	ModelLoader m_Loader;

	ComPtr<ID3D11Device> m_Device;
	ComPtr<ID3D11DeviceContext> m_Context;

	ConstantBuffer<cbPerObject> m_ConstantBuffer;
	ConstantBuffer<cbNormalMapping> m_cbMaterial;
	
	ConstantBuffer<cbPBRMaterial> m_cbPBR;

	XMMATRIX m_WorldMatrix;
	XMMATRIX m_RotationMatrix;

protected:
	std::array<float, 4> m_Ambient;
	std::array<float, 3> m_Diffuse;
	std::array<float, 3> m_Distances;
	float m_BlendAll;
	static float Intensity;

	float Metallic = 1.0f;
	float Roughness = 1.0f;

	// Model Transforms
	XMVECTOR m_Translation;
	XMVECTOR m_Rotation;
	XMVECTOR m_Scale;
	std::array<float, 3> m_Translations;
	std::array<float, 3> m_Rotations;
	std::array<float, 3> m_Scales;
	void UpdateWorld();
	
public:
	static float rotY;
	bool bShouldRotateY = false;
	void RotateY();

private:
	std::string m_ModelName = "Model";

	Camera CameraRef;

public:
	// Specular light test
	ConstantBuffer<cbSpecularLight> m_cbSpecular;
	ConstantBuffer<cbCameraBuffer> m_cbCamera;
	float SpecularIntensity = 32.0f;
	std::array<float, 4> m_Specular = { 1.0f, 1.0f, 1.0f, 1.0f };

};

