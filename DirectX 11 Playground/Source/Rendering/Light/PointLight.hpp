#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class PointLight
{
public:

private:
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_Context;

	//ID3D11InputLayout

	float m_Radius;
	XMFLOAT3 m_Position;
};

