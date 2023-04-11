
cbuffer Material : register(b0)
{
    float4 Ambient;
    float4 Diffuse;
    
    float Intensity;
    float Range;
    float ConeAngle;
    float Padding;
    
    float Attenuation_A;
    float Attenuation_B;
    float Attenuation_C;
    float Padding2;
    
    float4 Position;
    float4 Direction;
    
    bool bHasNormals;
};

struct PS_INPUT
{
    float4 Position  : SV_POSITION;
    float3 WorldPos  : WORLD_POSITION;
    float2 UVs       : TEXCOORD;
    float3 Normal    : NORMAL;
    float3 Tangent   : TANGENT;
    float3 Bitangent : BITANGENT;
};

Texture2D baseTexture : register(t0);
SamplerState baseSampler : register(s0);

Texture2D normalTexture : register(t1);

float4 PS(PS_INPUT pin) : SV_TARGET
{
    float4 baseColor = baseTexture.Sample(baseSampler, pin.UVs);
    
    float4 output = baseColor;
    float3 colorOutput = float3(0.0f, 0.0f, 0.0f);
    
    // Vector between Light Position and Pixel (Model) Position
    float3 lightToPixel = Position.xyz - pin.WorldPos.xyz;
    
    float distance = length(lightToPixel);
    
    float3 ambient = baseColor.rgb * Ambient.xyz;
    
    if (distance > Range)
        return float4(ambient.rgb, Ambient.a);
    
    lightToPixel /= distance;
    
    float lightIntensity = dot(lightToPixel, pin.Normal);
    if (lightIntensity > 0.0f)
    {
        colorOutput += baseColor * Diffuse;
        
        colorOutput /= (Attenuation_A + (Attenuation_B * distance) + (Attenuation_C * (distance * distance)));
        
        //Calculate falloff from center to edge of cone
        colorOutput *= pow(max(dot(-lightToPixel, Direction.xyz), 0.0f), ConeAngle);
    }
    
    if (bHasNormals)
    {
        float4 normalOutput = normalTexture.Sample(baseSampler, pin.UVs);
        normalOutput = normalize((2.0f * normalOutput) - 1.0f);
        
        float3 tangent = normalize(pin.Tangent - dot(pin.Tangent, pin.Normal) * pin.Normal);
        float3 bitangent = cross(pin.Normal, pin.Tangent);
        float3x3 texSpace = float3x3(tangent, bitangent, pin.Normal);
        pin.Normal = normalize(mul(normalOutput.xyz, texSpace));
    }

    colorOutput += saturate(dot(lightToPixel.xyz, pin.Normal) * (colorOutput * Intensity + ambient));
    
    return float4(colorOutput.rgb, Ambient.a);
}