// https://alexanderameye.github.io/notes/rendering-outlines/

cbuffer Material : register(b0)
{
    float4 Ambient;
    float3 Diffuse;
    float pad;
    float3 Direction;
    float pad2;
    float4 paddingForSpecular;
    float Power;
};

struct Input
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : WORLD_POSITION;
    float3 ViewDirection : VIEW_DIRECTION;
    float2 UVs : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

Texture2D baseTexture : register(t0);
Texture2D normalTexure : register(t1);

SamplerState baseSampler : register(s0);

float4 PS(Input pin) : SV_TARGET
{
    float3 diffuse = baseTexture.Sample(baseSampler, pin.UVs).rgb;
    float3 normalMap = normalTexure.Sample(baseSampler, pin.UVs).rgb;
    normalMap = normalize(2.0f * normalMap - 1.0f);
    
    float3 normal = pin.Normal;
    float3 tangent = normalize(pin.Tangent - dot(pin.Tangent, normal) * normal);
    float3 bitangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    //pin.Normal = normalize(mul(normalMap, TBN));
    
    float3 ambient = diffuse.rgb * Ambient.xyz;
    float3 output = ambient;
    output *= pow(1.0f - saturate(dot(pin.Normal, pin.ViewDirection)), Power);

    //output += saturate(dot(pin.Normal, Direction.xyz) * diffuse.rgb);
    //float3 output = pow(1.0f - saturate(dot(pin.Normal, pin.ViewDirection)), Power);
    //float3 output = pow(1.0f - saturate(dot(pin.Normal, pin.ViewDirection)), Power);
    //output *= ambient;
    
	return float4(output, 1.0f);
}