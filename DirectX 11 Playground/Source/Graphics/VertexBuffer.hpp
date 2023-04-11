#pragma once
#include <d3d11.h>
#include "Vertex.hpp"

template<typename T>
class VertexBuffer
{
public:
	VertexBuffer() { BufferSize = 0; }
	VertexBuffer(const VertexBuffer& rhs)
	{
		this->Buffer = rhs.Buffer;
		this->BufferSize = rhs.BufferSize;
		this->Stride = rhs.Stride;
	}
	VertexBuffer<T>& operator=(const VertexBuffer<T>& rhs)
	{
		this->Buffer = rhs.Buffer;
		this->BufferSize = rhs.BufferSize;
		this->Stride = rhs.Stride;

		return *this;
	}
	~VertexBuffer() { Release(); }

	ID3D11Buffer* GetBuffer() const { return Buffer.Get(); }
	ID3D11Buffer* const* GetAddressOf() const { return Buffer.GetAddressOf(); }

	const uint32_t  GetStride() const { return Stride; }
	const uint32_t* GetStridePtr() const { return &Stride; }
	const uint32_t	GetBufferSize() const { return BufferSize; }
	const uint32_t	GetSizeT() const { return static_cast<uint32_t>(sizeof(T)); }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	uint32_t Stride{ static_cast<uint32_t>(sizeof(T)) };
	uint32_t BufferSize{ 0 };

public:
	
	HRESULT Create(ID3D11Device* pDevice, T* Data, size_t VerticesCount)
	{
		BufferSize = static_cast<uint32_t>(VerticesCount);

		D3D11_BUFFER_DESC desc{};
		desc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
		desc.Usage			= D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth		= sizeof(T) * static_cast<uint32_t>(VerticesCount);
		desc.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA sub{};
		sub.pSysMem = Data;

		return pDevice->CreateBuffer(&desc, &sub, Buffer.GetAddressOf());
	}

	void Release()
	{
		Buffer.Reset();
		Buffer = nullptr;
	}
};
