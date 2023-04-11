
cbuffer cbMaterial : register(b0)
{
    float4 BaseColor;
    float3 DiffuseColor;
    float padding;
    float3 Direction;
    float padding2;
    float4 SpecularColor;
    float SpecularIntensity;
}

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

Texture2D baseTexture               : register(t0);
Texture2D normalTexture             : register(t1);
Texture2D metallicRoughnessTexture  : register(t2);
Texture2D emissiveTexture           : register(t3);
SamplerState objSamplerState        : register(s0);

float4 PS(PS_Input pin) : SV_TARGET
{
    float4 tex = baseTexture.Sample(objSamplerState, pin.TexCoord);
    if (tex.a < 0.1f)
        discard;

    //float3 gamma = float3(1.375f, 1.375f, 1.375f);
    //float3 gamma = float3(0.4545f, 0.4545f, 0.4545f);
    float3 baseColor= baseTexture.Sample(objSamplerState, pin.TexCoord).rgb * BaseColor.xyz;
    //output = pow(output, gamma);

    float3 colorOutput = baseColor;
    
    float4 normal = normalize(2.0f * normalTexture.Sample(objSamplerState, pin.TexCoord) - 1.0f);
    
    float3 tangent          = normalize(pin.Tangent - dot(pin.Tangent, pin.Normal) * pin.Normal);
    float3 bitangent        = cross(pin.Normal, tangent);
    float3x3 tangentSpace   = float3x3(tangent, bitangent, pin.Normal);
    
    pin.Normal = normalize(mul(normal.xyz, tangentSpace));
    
    colorOutput += saturate(dot(pin.Normal, Direction.xyz) * DiffuseColor.xyz * BaseColor.xyz);
    colorOutput *= baseColor.rgb;
    
    return float4(colorOutput.rgb, 1.0f);
}