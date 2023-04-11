
cbuffer cbPerObject : register(b0)
{
    row_major float4x4 WVP;
    row_major float4x4 World;
};

struct VS_INPUT
{
    float3 Position  : POSITION;
    float2 UVs       : TEXCOORD;
    float3 Normal    : NORMAL;
    float3 Tangent   : TANGENT;
    float3 Bitangent : BITANGENT;
};

struct VS_OUTPUT
{
    float4 Position  : SV_POSITION;
    float2 UVs       : TEXCOORD;
    float3 Normal    : NORMAL;
    float3 Tangent   : TANGENT;
    float3 Bitangent : BITANGENT;
};


VS_OUTPUT VS(VS_INPUT vin)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.Position = mul(WVP, float4(vin.Position, 1.0f));
    output.UVs = vin.UVs;
    output.Normal = normalize(mul((float3x3) World, vin.Normal));
    output.Tangent = normalize(mul((float3x3) World, vin.Tangent));
    output.Bitangent = normalize(mul((float3x3) World, vin.Bitangent));

    return output;
}