
cbuffer Light : register(b0)
{
    float4 Ambient;
    float3 Diffuse;
    float padding;
    float3 Direction;
    float padding2;
    float4 Specular;
    float SpecularIntensity;
};

struct PS_Input
{
    float4 Position     : SV_POSITION;
    float4 WorldPosition : WORLD_POSITION;
    float3 ViewDirection : VIEW_DIRECTION;
    float2 UVs          : TEXCOORD;
    float3 Normal       : NORMAL;
    float3 Tangent      : TANGENT;
    float3 Bitangent    : BITANGENT;
};

Texture2D baseTexture   : register(t0);

SamplerState baseSampler : register(s0);

float4 PS(PS_Input pin) : SV_TARGET
{
    
    float4 diffuse = baseTexture.Sample(baseSampler, pin.UVs);

    if (diffuse.a < 0.1f)
        discard;
    
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 lightDir = -Direction;
    
    float lightIntensity = saturate(dot(normalize(pin.Normal), lightDir));
    
    float3 output = diffuse.rgb;
    if (lightIntensity > 0.0f)
    {
        output += (Diffuse * lightIntensity);
        output = saturate(output);
        
        float3 reflection = normalize(2 * lightIntensity * pin.Normal - lightDir);
        specular = pow(saturate(dot(reflection, pin.ViewDirection)), SpecularIntensity) * Specular;
    }
    
    output = output * diffuse.rgb;
    
    output = saturate(output + specular.xyz);
    
	return float4(output, Ambient.a);
}