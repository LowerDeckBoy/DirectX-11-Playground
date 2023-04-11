#pragma once
#include "D3D.hpp"

#include "../Graphics/IndexBuffer.hpp"
#include "../Graphics/VertexBuffer.hpp"
#include "../Graphics/ConstantBuffer.hpp"

#include "../Graphics/Shader.hpp"
//#include "../Rendering/Model.hpp"

#include "../Rendering/Model/cglTF_Model.hpp"


class Renderer : public D3D
{
public:
	Renderer(HINSTANCE hInstance);
	Renderer(const Renderer&) = delete;
	Renderer operator=(const Renderer&) = delete;
	~Renderer() override;

	bool Initialize();
	void InitShaders();

	void OnResize() final;
	void CameraInputs(float DeltaTime) final;
	void Update(float DeltaTime) final;
	void Render() final;

	void DrawGUI();

	static inline int32_t SelectedPixel{ 0 };
	ID3D11PixelShader* SetPixelShader(int32_t& Selected);

private:
	std::unique_ptr<shader::VertexShader> m_GlobalVS		= std::make_unique<shader::VertexShader>();
	std::unique_ptr<shader::PixelShader> m_GlobalPS			= std::make_unique<shader::PixelShader>();
	std::unique_ptr<shader::PixelShader> m_BaseColorPS		= std::make_unique<shader::PixelShader>();
	std::unique_ptr<shader::VertexShader> m_SpecularVS		= std::make_unique<shader::VertexShader>();
	std::unique_ptr<shader::PixelShader> m_SpecularPS		= std::make_unique<shader::PixelShader>();
	std::unique_ptr<shader::VertexShader> m_SpecularMapVS	= std::make_unique<shader::VertexShader>();
	std::unique_ptr<shader::PixelShader> m_SpecularMapPS	= std::make_unique<shader::PixelShader>();

	std::unique_ptr<shader::PixelShader> m_TestPS = std::make_unique<shader::PixelShader>();


	ComPtr<ID3D11InputLayout> m_Layout;

	// glTFs go here
	cglTF_Model m_Model;

	Texture* m_HDRTest;

};

