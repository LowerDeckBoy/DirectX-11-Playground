
cbuffer cbPerObject : register(b0)
{
    row_major float4x4 WVP;
    row_major float4x4 World;
};

struct VS_Input
{
    float3 Position : POSITION;
    float3 Color : COLOR;
};

struct VS_Output
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

VS_Output main(VS_Input vin)
{
    VS_Output output = (VS_Output) 0;
    output.Position = mul(WVP, float4(vin.Position, 1.0f));
    output.Color = float4(vin.Color, 1.0f);
    
    return output;
}