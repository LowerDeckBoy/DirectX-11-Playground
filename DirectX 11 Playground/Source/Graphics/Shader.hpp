#pragma once
#include <d3dcompiler.h>
#include <d3d11.h>

namespace shader
{
	using Microsoft::WRL::ComPtr;
	class VertexShader
	{
	public:
		VertexShader() { }

		ID3D11VertexShader* GetShader() const { return m_Shader.Get(); }

		HRESULT Create(LPCWSTR pFilepath, LPCSTR pEntrypoint, ID3D11Device* pDevice)
		{
			UINT shaderFlags = 0;
#if defined (_DEBUG) || (DEBUG)
			shaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

			HRESULT hResult = D3DCompileFromFile(pFilepath, 0, 0, pEntrypoint, "vs_5_0", shaderFlags, 0, Blob.GetAddressOf(), Error.GetAddressOf());

			if (FAILED(hResult))
			{
				::OutputDebugStringA("Failed to create Vertex Shader From File!\n");
				if (Blob == nullptr)
				{
					::OutputDebugStringA("Vertex Shader Blob is nullptr!\n");
				}
				if (Error != nullptr)
				{
					::OutputDebugStringA((char*)Error->GetBufferPointer());
				}
				return E_FAIL;
			}

			return pDevice->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), nullptr, m_Shader.GetAddressOf());
		}

		void Release()
		{
			m_Shader.Reset();
			Blob = nullptr;
			Error = nullptr;
		}

		ComPtr<ID3DBlob> Blob;
	private:
		ComPtr<ID3D11VertexShader> m_Shader;
		ComPtr<ID3DBlob> Error;
	};

	class PixelShader
	{
	public:
		PixelShader() { }

		ID3D11PixelShader* GetShader() const { return m_Shader.Get(); }

		HRESULT Create(LPCWSTR pFilepath, LPCSTR pEntrypoint, ID3D11Device* pDevice)
		{
			UINT shaderFlags = 0;
#if defined (_DEBUG) || (DEBUG)
			shaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

			HRESULT hResult = D3DCompileFromFile(pFilepath, 0, 0, pEntrypoint, "ps_5_0", shaderFlags, 0, Blob.GetAddressOf(), Error.GetAddressOf());

			if (FAILED(hResult))
			{
				::OutputDebugStringA("Failed to create Pixel Shader From File!\n");
				if (Blob == nullptr)
				{
					::OutputDebugStringA("Pixel Shader Blob is nullptr!\n");
				}
				if (Error != nullptr)
				{
					::OutputDebugStringA((char*)Error->GetBufferPointer());
				}
				return E_FAIL;
			}

			return pDevice->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), nullptr, m_Shader.GetAddressOf());
		}

		void Release()
		{
			m_Shader.Reset();
			Blob = nullptr;
			Error = nullptr;
		}

	private:
		ComPtr<ID3D11PixelShader> m_Shader;
		ComPtr<ID3DBlob> Blob;
		ComPtr<ID3DBlob> Error;
	};

}
