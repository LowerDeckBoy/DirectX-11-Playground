
cbuffer cbPerObject : register(b0)
{
    float4x4 WVP;
    float4x4 World;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};


VS_OUTPUT VS(VS_INPUT vin)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.Position = mul(WVP, float4(vin.Position, 1.0f));
    output.UV = vin.UV;
    output.Normal = normalize(mul(World, float4(vin.Normal, 0.0f)));
    output.Tangent = vin.Tangent;
    
    return output;
}
