#pragma once

class IndexBuffer
{
public:
	IndexBuffer() = default;
	IndexBuffer(const IndexBuffer& rhs) {}
	~IndexBuffer() { Release(); }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	uint32_t BufferSize{ 0 };

public:
	ID3D11Buffer* GetBuffer() const { return Buffer.Get(); }
	ID3D11Buffer* const* GetAddressOf() const { return Buffer.GetAddressOf(); }

	uint32_t GetBufferSize() const { return BufferSize; }
	
	HRESULT Create(ID3D11Device* Device, uint32_t* Data, size_t IndicesCount)
	{
		BufferSize = static_cast<uint32_t>(IndicesCount);

		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(IndicesCount);
		desc.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA sub{};
		sub.pSysMem = Data;

		HRESULT hr = Device->CreateBuffer(&desc, &sub, Buffer.GetAddressOf());

		//return hr;
		return Device->CreateBuffer(&desc, &sub, Buffer.GetAddressOf());
	}

	// Potentially optionally unnecessary
	void Release()
	{
		Buffer.Reset();
		Buffer = nullptr;
	}
};
