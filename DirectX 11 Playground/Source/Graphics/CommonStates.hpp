#pragma once
#include <d3d11.h>

class CommonStates
{
private:
	static D3D11_RASTERIZER_DESC CreateRasterizerState(D3D11_CULL_MODE cull, D3D11_FILL_MODE fill);
	static D3D11_BLEND_DESC CreateBlendState(D3D11_BLEND srcBlend, D3D11_BLEND destBlend);
	static D3D11_DEPTH_STENCIL_DESC CreateDepthStencilState(bool enable, bool writeEnable, D3D11_COMPARISON_FUNC comparsion = D3D11_COMPARISON_LESS_EQUAL);
	static D3D11_SAMPLER_DESC CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);

public:
	static D3D11_RASTERIZER_DESC RS_Wireframe();
	static D3D11_RASTERIZER_DESC RS_Solid();
	static D3D11_RASTERIZER_DESC RS_CullClockwise();
	static D3D11_RASTERIZER_DESC RS_CullCounterClockwise();

	static D3D11_BLEND_DESC BlendAlpha();
	static D3D11_BLEND_DESC BlendOpaque();

	static D3D11_DEPTH_STENCIL_DESC DepthNone();
	static D3D11_DEPTH_STENCIL_DESC DepthDefault();
	static D3D11_DEPTH_STENCIL_DESC DepthApply();
	static D3D11_DEPTH_STENCIL_DESC DepthMask();

	static D3D11_SAMPLER_DESC PointWrap();
	static D3D11_SAMPLER_DESC PointClamp();
	static D3D11_SAMPLER_DESC LinearWrap();
	static D3D11_SAMPLER_DESC LinearClamp();
	static D3D11_SAMPLER_DESC AnisotropyWrap();
	static D3D11_SAMPLER_DESC AnisotropyClamp();
	static D3D11_SAMPLER_DESC AnisotropyMin();
	static D3D11_SAMPLER_DESC AnisotropyMax();
};

