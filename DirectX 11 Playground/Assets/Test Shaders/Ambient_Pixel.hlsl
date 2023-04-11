
struct PS_Input
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

Texture2D baseTexture : register(t0);
SamplerState baseSampler : register(s0);

float4 main(PS_Input pin) : SV_TARGET
{
    float3 output = baseTexture.Sample(baseSampler, pin.UV);
    
	return float4(output, 1.0f);
}