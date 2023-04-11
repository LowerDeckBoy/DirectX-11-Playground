#pragma once
#include "Window.hpp"
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3d11sdklayers.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "../Utils/Utils.hpp"
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>
#include "../Rendering/Camera.hpp"
#include "../Graphics/CommonStates.hpp"

#include <memory>

using Microsoft::WRL::ComPtr;

class D3D : public Window
{
public:
	D3D(HINSTANCE hInstance);
	D3D(const D3D&) = delete;
	~D3D();

	bool Initialize();
	bool InitializeD3D();

	LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	void OnResize() override;

	// virtuals
	void Inputs(float DeltaTime) {}
	void Update(float DeltaTime) {}
	void Render() {}

	void InitializeStates();
	void SetBackbuffer();
	void ClearBackbuffer(const float background[4]);

	bool bSolidState = true;
	ID3D11RasterizerState* SetRasterizerState() const;

	// Getters
	ID3D11Device5* GetDevice() const { return m_Device.Get(); }
	ID3D11DeviceContext4* GetContext() const { return m_Context.Get(); }
	IDXGISwapChain4* GetSwapChain() { return m_SwapChain.Get(); }
	ID3D11RenderTargetView* GetRenderTargetView() { return m_RenderTargetView.Get(); }
	ID3D11RenderTargetView* const* GetRenderTargetViewAddress() { return m_RenderTargetView.GetAddressOf(); }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_DepthStencilView.Get(); }
	ID3D11Texture2D* GetDepthStencilBuffer() const { return m_DepthStencilBuffer.Get(); }

	ID3D11RasterizerState* GetRasterizerSolid() const { return m_RasterizerSolid.Get(); }
	ID3D11RasterizerState* GetRasterizerWireframe() const { return m_RasterizerWireframe.Get(); }

	ID3D11SamplerState* const* SetMainSamplerState(uint32_t Select) const;
	ID3D11SamplerState* const* SetSecondSamplerState(uint32_t Select) const;
	inline static int SelectedMainSampler	= 0;
	inline static int SelectedSecondSampler = 0;

	[[maybe_unused]]
	ID3D11DepthStencilState* const* SetDepthStencilState(int Selected) const;
	static int SelectedDSS;

private:
	ComPtr<ID3D11Device5> m_Device;
	ComPtr<ID3D11DeviceContext4> m_Context;
	ComPtr<IDXGISwapChain4> m_SwapChain;
	ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
	ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;

protected:
	// Rasterizers
	ComPtr<ID3D11RasterizerState> m_RasterizerSolid;
	ComPtr<ID3D11RasterizerState> m_RasterizerWireframe;

	// Samplers
	ComPtr<ID3D11SamplerState> m_SSPointWrap;
	ComPtr<ID3D11SamplerState> m_SSPointClamp;
	ComPtr<ID3D11SamplerState> m_SSLinearWrap;
	ComPtr<ID3D11SamplerState> m_SSLinearClamp;
	ComPtr<ID3D11SamplerState> m_SSAnisotropyWrap;
	ComPtr<ID3D11SamplerState> m_SSAnisotropyClamp;
	ComPtr<ID3D11SamplerState> m_SSAnisotropyMin;
	ComPtr<ID3D11SamplerState> m_SSAnisotropyMax;

	ComPtr<ID3D11BlendState> m_BlendAlpha;
	ComPtr<ID3D11BlendState> m_BlendOpaque;

	ComPtr<ID3D11DepthStencilState> m_DSSDefault;
	ComPtr<ID3D11DepthStencilState> m_DSSNone;


protected:
	IDirectInputDevice8* DX_Keyboard{};
	IDirectInputDevice8* DX_Mouse{};
	LPDIRECTINPUT8 DX_Input{};
	DIMOUSESTATE DX_LastMouseState{};

	//Camera* m_Camera;
	std::unique_ptr<Camera> m_Camera = std::make_unique<Camera>();

	// To bind Cascadia Code Font
	ImFont* m_MainFont;
};

