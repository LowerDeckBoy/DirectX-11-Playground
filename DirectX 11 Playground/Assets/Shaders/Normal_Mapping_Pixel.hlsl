
cbuffer Material : register(b0)
{
    float4 Ambient;
    float3 Diffuse;
    float pad;
    float3 Direction;
    float pad2;
    float4 paddingForSpecular;
    float padding;
    //bool bHasDiffuse;
    //bool bHasNormals;
    //bool bHasMetallic;
    //float Intensity;
};

struct PS_Input
{
    float4 Position         : SV_POSITION;
    float4 WorldPosition    : WORLD_POSITION;
    float3 ViewDir          : VIEW_DIRECTION;
    float2 TexCoord         : TEXCOORD;
    float3 Normal           : NORMAL;
    float3 Tangent          : TANGENT;
    float3 Bitangent        : BITANGENT;
};

Texture2D baseTexture     : register(t0);
Texture2D normalTexture   : register(t1);
Texture2D metallicTexture : register(t2);
Texture2D emissiveTexture : register(t3);

SamplerState baseSamplerState   : register(s0);
SamplerState secondSamplerState : register(s1);


float4 PS(PS_Input pin) : SV_TARGET
{
    pin.Normal = normalize(pin.Normal);
      
    float4 baseOutput = baseTexture.Sample(baseSamplerState, pin.TexCoord);
    
    // Needed for getting off texure boxes around leaves
    if (baseOutput.a < 0.1f)
    {
        discard;
    }
  
    float3 ambient = baseOutput.rgb * Ambient.xyz;
    
    float3 colorOutput = ambient.rgb;
    
    float4 normalOutput = normalTexture.Sample(secondSamplerState, pin.TexCoord);
    normalOutput = normalize(2.0f * normalOutput - 1.0f);
          
    float3 tangent = normalize(pin.Tangent - dot(pin.Tangent, pin.Normal) * pin.Normal);
    float3 bitangent = cross(pin.Normal, tangent);
    float3x3 texSpace = float3x3(tangent, bitangent, pin.Normal);
    
    pin.Normal = normalize(mul(normalOutput.xyz, texSpace));
    
    colorOutput += saturate(dot(pin.Normal, -Direction.xyz) * Diffuse.xyz * ambient.rgb);
    colorOutput *= ambient.rgb;

    return float4(colorOutput.rgb, Ambient.a);
}