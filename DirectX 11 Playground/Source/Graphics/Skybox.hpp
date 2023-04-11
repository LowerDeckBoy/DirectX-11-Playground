#pragma once

#include <DirectXTex.h>
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "ConstantBuffer.hpp"
#include <memory>

using namespace DirectX;

class Skybox
{
public:
	Skybox();

	bool Initialize();

private:
	struct Skybox_Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT2 TexCoord;
	};
	
	std::unique_ptr<VertexBuffer<
	

};