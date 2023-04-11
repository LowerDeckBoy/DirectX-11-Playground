#pragma once
#include "ConstantTypes.hpp"

template<typename T>
class ConstantBuffer
{
public:
	ConstantBuffer() = default;
	~ConstantBuffer() { Release(); }
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;

public:
	T Data{};

	ID3D11Buffer* const  GetBuffer() const { return Buffer.Get(); }
	ID3D11Buffer* const* GetAddressOf() const { return Buffer.GetAddressOf(); }

	HRESULT Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	{
		Context = pContext;

		D3D11_BUFFER_DESC desc{};
		desc.Usage	= D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.ByteWidth = static_cast<uint32_t>(sizeof(T) + (16 - (sizeof(T) % 16)));

		return pDevice->CreateBuffer(&desc, nullptr, Buffer.GetAddressOf());
	}

	void UpdateBuffer(const T& Updated)
	{
		Data = Updated;
		Context->UpdateSubresource(Buffer.Get(), 0, nullptr, &Data, 0, 0);
	}

	void Release()
	{
		Buffer.Reset();
		Buffer = nullptr;
	}
	// for dynamic buffer
	bool AppyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource{};
		HRESULT hr = Context->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
			return false;

		CopyMemory(mappedResource.pData, &Data, sizeof(T));
		this->Context->Unmap(Buffer.Get(), 0);
		return true;
	}
};
