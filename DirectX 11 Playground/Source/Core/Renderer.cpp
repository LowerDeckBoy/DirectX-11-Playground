#include "Renderer.hpp"
#include <array>
#if defined (_DEBUG) || (DEBUG)
#include "../Utils/RAMUsage.hpp"
#endif

std::array<float, 4> Background = { 0.1f, 0.1f, 0.1f, 1.0f };

Renderer::Renderer(HINSTANCE hInstance) : D3D(hInstance)
{

}

Renderer::~Renderer()
{
	ReleaseCOM(m_Layout);

}

bool Renderer::Initialize()
{
	if (!D3D::Initialize())
		return false;

	InitShaders();
	m_Camera->Setup(GetDisplayResolution().AspectRatio);
	m_HDRTest = new Texture(GetDevice(), GetContext(), "Assets/Skybox/HDR/PaperMill_Ruins_E/PaperMill_E_Env.hdr");

	//m_Model.Create(GetDevice(), GetContext(), "Assets/glTF/sponza/scene.gltf");
	//m_Model.Create(GetDevice(), GetContext(), "Assets/glTF/mathilda/scene.gltf");
	//m_Model.Create(GetDevice(), GetContext(), "Assets/glTF/fallout_ranger/scene.gltf");
	//m_Model.Create(GetDevice(), GetContext(), "Assets/glTF/crusader_knight/scene.gltf");
	m_Model.Create(GetDevice(), GetContext(), "Assets/glTF/timothy/scene.gltf");


  	return true;
}

void Renderer::InitShaders()
{
	ThrowIfFailed(m_GlobalVS->Create(L"Assets/Shaders/Global_Vertex.hlsl", "VS", GetDevice()));
	ThrowIfFailed(m_GlobalPS->Create(L"Assets/Shaders/Normal_Mapping_Pixel.hlsl",  "PS", GetDevice()));
	ThrowIfFailed(m_BaseColorPS->Create(L"Assets/Shaders/Base_Pixel.hlsl",  "PS", GetDevice()));
	//ThrowIfFailed(m_BaseColorPS->Create(L"Assets/Shaders/Test/TEST_PIXEL.hlsl",  "PS", GetDevice()));
	ThrowIfFailed(m_SpecularPS->Create(L"Assets/Shaders/Specular_Light_Pixel.hlsl", "PS", GetDevice()));
	ThrowIfFailed(m_SpecularMapPS->Create(L"Assets/Shaders/SpecularMap_Pixel.hlsl", "PS", GetDevice()));

	ThrowIfFailed(m_TestPS->Create(L"Assets/Shaders/Test/TEST_PIXEL.hlsl", "PS", GetDevice()));

	std::array<D3D11_INPUT_ELEMENT_DESC, 5> layout =
	{
		D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		D3D11_INPUT_ELEMENT_DESC{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		D3D11_INPUT_ELEMENT_DESC{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		D3D11_INPUT_ELEMENT_DESC{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		D3D11_INPUT_ELEMENT_DESC{"BITANGENT",0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	ThrowIfFailed(GetDevice()->CreateInputLayout(layout.data(), static_cast<uint32_t>(layout.size()), 
												 m_GlobalVS->Blob->GetBufferPointer(), 
												 m_GlobalVS->Blob->GetBufferSize(), 
												 m_Layout.GetAddressOf()));

	GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::OnResize()
{
	D3D::OnResize();
}

// Mouse and keyboard binding
void Renderer::CameraInputs(float DeltaTime)
{
	DIMOUSESTATE mouseState{};
	const int keys = 256;
	std::array<BYTE, keys> keyboardState{};

	DX_Mouse->Acquire();
	DX_Mouse->GetDeviceState(sizeof(mouseState), (LPVOID)&mouseState);
	DX_Keyboard->Acquire();
	DX_Keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	const int state = 0x80;
	if (keyboardState[DIK_ESCAPE] & state)
	{
		GetSwapChain()->SetFullscreenState(FALSE, NULL);
		PostMessage(GetHWND(), WM_DESTROY, 0, 0);
	}

	if (!mouseState.rgbButtons[1])
	{
		ShowCursor();
		return;
	}

	HideCursor();

	float speed = m_Camera->CameraSpeed * static_cast<float>(DeltaTime);
	const float intensity = 0.001f;
	const float upDownIntensity = 0.75f;

	if ((mouseState.lX != DX_LastMouseState.lX) || (mouseState.lY != DX_LastMouseState.lY))
	{
		m_Camera->Yaw += mouseState.lX * intensity;
		m_Camera->Pitch += mouseState.lY * intensity;
		DX_LastMouseState = mouseState;
	}
	if (keyboardState[DIK_A] & state)
	{
		m_Camera->MoveLeftRight -= speed;
	}
	if (keyboardState[DIK_D] & state)
	{
		m_Camera->MoveLeftRight += speed;
	}
	if (keyboardState[DIK_W] & state)
	{
		m_Camera->MoveBackForward += speed;
	}
	if (keyboardState[DIK_S] & state)
	{
		m_Camera->MoveBackForward -= speed;
	}
	if (keyboardState[DIK_Q] & state)
	{
		m_Camera->MoveUpDown -= speed * upDownIntensity;
	}
	if (keyboardState[DIK_E] & state)
	{
		m_Camera->MoveUpDown += speed * upDownIntensity;
	}

	m_Camera->Update();
}

void Renderer::Update(float DeltaTime)
{
	
}

void Renderer::Render()
{
	SetBackbuffer();
	ClearBackbuffer(Background.data());

	GetContext()->IASetInputLayout(m_Layout.Get());
	const int Black = 0xffffff;
	GetContext()->OMSetBlendState(m_BlendAlpha.Get(), 0, Black);

	//GetContext()->OMSetDepthStencilState(m_DSSDefault.Get(), 0);
	//GetContext()->OMSetDepthStencilState(m_DSSNone.Get(), 0);

	GetContext()->RSSetState(SetRasterizerState());
	GetContext()->PSSetSamplers(0, 1, SetMainSamplerState(D3D::SelectedMainSampler));
	GetContext()->PSSetSamplers(1, 1, SetSecondSamplerState(D3D::SelectedSecondSampler));

	GetContext()->VSSetShader(m_GlobalVS->GetShader(), nullptr, 0);
	GetContext()->PSSetShader(SetPixelShader(SelectedPixel), nullptr, 0);

	// Put objects here
	{
		m_Model.Draw(m_Camera.get());
	}

	{

	}

	DrawGUI();
	ThrowIfFailed(GetSwapChain()->Present(1, 0));
}

void Renderer::DrawGUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::PushFont(m_MainFont);
	
	{
		ImGui::Begin("UI");

		if (ImGui::CollapsingHeader("Stats"))
		{
			ImGui::Text("FPS: %.2d ms: %.2f", GetTimer()->m_FPS, GetTimer()->m_Miliseconds);
			ImGui::Text("Resolution: %.0fx%.0f", GetDisplayResolution().Width, GetDisplayResolution().Height);
			ImGui::Text("Time elapsed: %.2f", GetTimer()->m_TimeElapsed);
#if defined (_DEBUG) || (DEBUG)
			RAMUsage::ReadRAM();
			ImGui::Text("Available RAM: %d / %d MB", (uint32_t)RAMUsage::AvailableRAM, (uint32_t)RAMUsage::TotalRAM);
			ImGui::Text("Memory usage: %.3f MB", RAMUsage::MemoryUsed);
#endif
			ImGui::Text(""); // Padding
		}

		if (ImGui::CollapsingHeader("Background"))
		{
			ImGui::ColorEdit4("Color", Background.data());
		}

		if (ImGui::CollapsingHeader("States"))
		{
			ImGui::Text("Rasterizer State ");
			ImGui::SameLine();
			if (bSolidState ? ImGui::Button("Solid") : ImGui::Button("Wireframe"))
				bSolidState ^= true;

			ImGui::Text("Sampler State no.1");
			// TODO: Change states to enum class
			const std::array<const char*, 8> states_01 = {
				"MIN_MAG_MIP_POINT WRAP", "MIN_MAG_MIP_POINT CLAMP",
				"MIN_MAG_MIP_LINEAR WRAP", "MIN_MAG_MIP_LINEAR CLAMP",
				"D3D11_FILTER_ANISOTROPIC WRAP", "D3D11_FILTER_ANISOTROPIC CLAMP",
				"D3D11_FILTER_ANISOTROPIC MIN", "D3D11_FILTER_ANISOTROPIC MAX"
			};
			ImGui::ListBox("First sampler", &D3D::SelectedMainSampler, states_01.data(), (int)states_01.size());

			ImGui::Text("Sampler State no.2");
			const std::array<const char*, 8> states_02 = {
				"MIN_MAG_MIP_POINT WRAP", "MIN_MAG_MIP_POINT CLAMP",
				"MIN_MAG_MIP_LINEAR WRAP", "MIN_MAG_MIP_LINEAR CLAMP",
				"D3D11_FILTER_ANISOTROPIC WRAP", "D3D11_FILTER_ANISOTROPIC CLAMP",
				"D3D11_FILTER_ANISOTROPIC MIN", "D3D11_FILTER_ANISOTROPIC MAX"
			};

			ImGui::ListBox("Second sampler", &D3D::SelectedSecondSampler, states_02.data(), (int)states_02.size());
		}

		if (ImGui::CollapsingHeader("Pixel Shader"))
		{
			ImGui::Text("Shader");

			const std::array<const char*, 5> shaders = { 
				"Base Color",
				"Normal Map",
				"Specular Light",
				"Specular Map",
				"Test Shader"
			};

			ImGui::ListBox("Shader: ", &Renderer::SelectedPixel, shaders.data(), (int32_t)shaders.size());
		}

		ImGui::End();
	}

	{
		ImGui::Begin("Controls");
		
		if (ImGui::CollapsingHeader("Camera"))
		{
			m_Camera->DrawGUI();
		}

		if (ImGui::CollapsingHeader("Model"))
		{
			m_Model.DrawGUI();
		}

		ImGui::End();
	}

	ImGui::PopFont();

	ImGui::Render();
	//ImGui::UpdatePlatformWindows();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

ID3D11PixelShader* Renderer::SetPixelShader(int32_t& Selected)
{
	switch (Selected)
	{
	case 0:
		return m_BaseColorPS->GetShader();
	case 1:
		return m_GlobalPS->GetShader();
	case 2:
		return m_SpecularPS->GetShader();
	case 3:
		return m_SpecularMapPS->GetShader();
	case 4:
		return m_TestPS->GetShader();
	//case 5:
	//	return m_PBR_TEST->GetShader();
	default:
		return m_GlobalPS->GetShader();
	}

	//return m_GlobalPS->GetShader();
}
