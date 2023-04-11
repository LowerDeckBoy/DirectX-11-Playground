#include "CommonStates.hpp"

D3D11_RASTERIZER_DESC CommonStates::CreateRasterizerState(D3D11_CULL_MODE cull, D3D11_FILL_MODE fill)
{
	D3D11_RASTERIZER_DESC desc{};
	desc.CullMode = cull;
	desc.FillMode = fill;
	desc.DepthClipEnable = TRUE;
	desc.MultisampleEnable = TRUE;

	return desc;
}

D3D11_BLEND_DESC CommonStates::CreateBlendState(D3D11_BLEND srcBlend, D3D11_BLEND destBlend)
{
	D3D11_BLEND_DESC desc{};

	desc.RenderTarget[0].BlendEnable = (srcBlend != D3D11_BLEND_ONE) || (destBlend != D3D11_BLEND_ZERO);
	desc.RenderTarget[0].SrcBlend = srcBlend;
	desc.RenderTarget[0].SrcBlendAlpha = srcBlend;
	desc.RenderTarget[0].DestBlend = destBlend;
	desc.RenderTarget[0].DestBlendAlpha = destBlend;

	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;


	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return desc;
}

D3D11_DEPTH_STENCIL_DESC CommonStates::CreateDepthStencilState(bool enable, bool writeEnable, D3D11_COMPARISON_FUNC comparsion)
{
	D3D11_DEPTH_STENCIL_DESC desc{};

	desc.DepthEnable = enable ? TRUE : FALSE;
	desc.DepthWriteMask = writeEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	//desc.DepthFunc = D3D11_COMPARISON_NEVER;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	desc.StencilEnable = TRUE;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	desc.FrontFace.StencilFunc		  = D3D11_COMPARISON_ALWAYS;
	desc.FrontFace.StencilPassOp	  = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp	  = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	desc.BackFace.StencilFunc		  = D3D11_COMPARISON_NEVER;
	desc.BackFace.StencilPassOp		  = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFailOp		  = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_KEEP;

	return desc;
}

D3D11_SAMPLER_DESC CommonStates::CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
{
	D3D11_SAMPLER_DESC desc{};
	desc.Filter = filter;
	desc.AddressU = addressMode;
	desc.AddressV = addressMode;
	desc.AddressW = addressMode;
	//desc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
	desc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	desc.MipLODBias = 0;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	//desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	return desc;
}

D3D11_RASTERIZER_DESC CommonStates::RS_Wireframe()
{
	return CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_WIREFRAME);
}

D3D11_RASTERIZER_DESC CommonStates::RS_Solid()
{
	return CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID);
}

D3D11_RASTERIZER_DESC CommonStates::RS_CullClockwise()
{
	return CreateRasterizerState(D3D11_CULL_FRONT, D3D11_FILL_SOLID);
}

D3D11_RASTERIZER_DESC CommonStates::RS_CullCounterClockwise()
{
	return CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID);
}

D3D11_BLEND_DESC CommonStates::BlendAlpha()
{
	return CreateBlendState(D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA);
}

D3D11_BLEND_DESC CommonStates::BlendOpaque()
{
	return CreateBlendState(D3D11_BLEND_ONE, D3D11_BLEND_ZERO);
}

D3D11_DEPTH_STENCIL_DESC CommonStates::DepthNone()
{
	return CreateDepthStencilState(false, false);
}

D3D11_DEPTH_STENCIL_DESC CommonStates::DepthDefault()
{
	return CreateDepthStencilState(true, true);
}

D3D11_SAMPLER_DESC CommonStates::PointWrap()
{
	return CreateSamplerState(D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
}

D3D11_SAMPLER_DESC CommonStates::PointClamp()
{
	return CreateSamplerState(D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
}

D3D11_SAMPLER_DESC CommonStates::LinearWrap()
{
	return CreateSamplerState(D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
}

D3D11_SAMPLER_DESC CommonStates::LinearClamp()
{
	return CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
}

D3D11_SAMPLER_DESC CommonStates::AnisotropyWrap()
{
	return CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
}

D3D11_SAMPLER_DESC CommonStates::AnisotropyClamp()
{
	return CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP);
}

D3D11_SAMPLER_DESC CommonStates::AnisotropyMin()
{
	return CreateSamplerState(D3D11_FILTER_MINIMUM_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
}

D3D11_SAMPLER_DESC CommonStates::AnisotropyMax()
{
	return CreateSamplerState(D3D11_FILTER_MAXIMUM_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
}
