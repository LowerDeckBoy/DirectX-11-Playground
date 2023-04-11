
struct PS_Input
{
    float4 Position         : SV_POSITION;
    float4 WorldPosition    : WORLD_POSITION;
    float3 ViewDirection    : VIEW_DIRECTION;
    float2 TexCoord         : TEXCOORD;
    float3 Normal           : NORMAL;
    float3 Tangent          : TANGENT;
    float3 Bitangent        : BITANGENT;
};


Texture2D    baseTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 PS(PS_Input pin) : SV_TARGET
{
    float4 tex = baseTexture.Sample(objSamplerState, pin.TexCoord);
    if (tex.a < 0.1f)
        discard;
    
    //float gamma = 2.2f / 1.6f;
    float gamma = 1.375;
    float3 output = pow(baseTexture.Sample(objSamplerState, pin.TexCoord).rgb, float3(gamma, gamma, gamma));
    //float3 output = baseTexture.Sample(objSamplerState, pin.TexCoord).rgb;
    return float4(output.rgb, 1.0f);
}