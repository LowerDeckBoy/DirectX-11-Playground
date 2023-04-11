//https://github.com/mateeeeeee/Adria-DX11/blob/master/Adria/Resources/Shaders/Deferred/GBuffer_PS.hlsl
// Pixel PBR Shader testing

// https://learnopengl.com/PBR/Theory
// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl

// Reference
// https://github.com/JJJohan/PBR/blob/master/PBR/PBR.shader

cbuffer pbrMaterial : register(b0)
{
    float3 Direction;
    float padding;
    float3 EyePosition;
    float padding2;
    
    float3 Diffuse;
    float padding3;
    
    float4 Ambient; // BaseColorFactor
    float MetallicFactor;
    float RoughnessFactor;
    float2 padding4;
    float4 EmissiveFactor;
    
    bool bHasDiffuse;
    bool bHasNormals;
    bool bHasMetallic;
};

static const float PI = 3.141592;
static const float Epsilon = 0.00001;

// Constant normal incidence Fresnel factor for all dielectrics.
static const float3 Fdielectric = 0.04;

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : WORLD_POS;
    float2 UVs : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};


float3 GetFresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

float3 GetFresnelSchlickRoughness(float cosTheta, float3 F0, float Roughness)
{
    return F0 + (max(float3(1.0f - Roughness, 1.0f - Roughness, 1.0f - Roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

// Normal Distribution
float GetDistributionGGX(float3 Normal, float3 H, float Roughness)
{
    float alpha = Roughness * Roughness;
    float alphaSq = alpha * alpha;
    float NdotH = max(dot(Normal, H), 0.0f);
    float NdotHsq = NdotH * NdotH;
    
    // Denormalize
    float denom = (NdotHsq * (alphaSq - 1.0f) + 1.0f);
    denom = PI * denom * denom;
    
    return alphaSq / denom;
}

float GetGeometrySchlickGGX(float NdotV, float Roughness)
{
    float r = (Roughness + 1.0f);
    float k = (r * r) / 8.0f;
    
    float denom = NdotV * (1.0f - k) + k;
    
    return NdotV / denom;
}

float GetGeometrySmith(float3 Normal, float3 V, float3 L, float Roughness)
{
    float NdotV = max(dot(Normal, V), 0.0f);
    float NdotL = max(dot(Normal, L), 0.0f);
    
    float ggx1 = GetGeometrySchlickGGX(NdotL, Roughness);
    float ggx2 = GetGeometrySchlickGGX(NdotV, Roughness);
    
    return ggx1 * ggx2;
}


//normal distribution
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float sqAlpha = alpha * alpha;
    
    float denom = (cosLh * cosLh) * (sqAlpha - 1.0f) + 1.0f;
    return sqAlpha / (PI * denom * denom);
}


Texture2D baseTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D metallicRoughnessTexture : register(t2);
Texture2D emissiveTexture : register(t3);

SamplerState baseSampler : register(s0);
//https://github.com/NPoole95/HLSL-shader-work-in-DX11
//https://marmoset.co/posts/physically-based-rendering-and-you-can-too/
//Global illumiation
float4 PS(PSInput pin) : SV_TARGET
{
    float4 baseColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float4 diffuseTex = baseTexture.Sample(baseSampler, pin.UVs);
    float3 normalMap = normalTexture.Sample(baseSampler, pin.UVs).rgb;
    float metallicFactor = metallicRoughnessTexture.Sample(baseSampler, pin.UVs).r;
    float roughnessFactor = metallicRoughnessTexture.Sample(baseSampler, pin.UVs).g;

    
    float3 normal = normalize(pin.Normal);
    //float3 tangent = normalize(pin.Tangent - dot(pin.Tangent, normal) * normal);
    //float3 bitangent = cross(normal, tangent);
    //float3x3 texSpace = float3x3(tangent, bitangent, normal);
    
    normalMap = normalize(2.0f * normalMap - 1.0f);
    
    float3 V = normalize(EyePosition - pin.WorldPosition.xyz);
    float3 _reflect = reflect(-V, normal);
    
    float3 F0 = float3(0.04, 0.04f, 0.04f);
    F0 = lerp(F0, diffuseTex.xyz, metallicFactor);
    
    // reflectance equation
    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    
    // light radiance
    float3 L = normalize(Direction.xyz - pin.WorldPosition.xyz);
    float3 H = normalize(V + L);
    
    float dist = length(Direction.xyz - pin.WorldPosition.xyz);
    float attenuation = 1.0f / (dist * dist);
    
    float3 radiance = Diffuse.rgb * attenuation;
    
    // cook-torrance brdf
    float3 NDF = GetDistributionGGX(normal, H, roughnessFactor);
    float G = GetGeometrySmith(normal, V, L, roughnessFactor);
    float3 Flick = GetFresnelSchlick(max(dot(H, V), 0.0f), F0);
    
    float3 kS = Flick;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= (1.0f - metallicFactor);
    
    float3 numerator = NDF * G * Flick;
    float denominator = 4.0f * max(dot(normal, V), 0.0f) * max(dot(normal, L), 0.0f);
    float3 specular = numerator / max(denominator, 0.001f);
    
    float NdotL = max(dot(normal, L), 0.0f);
    Lo += (kD * Ambient.rgb / PI + specular) * radiance * NdotL;
    
    //-------------------------------------------------------
    float flickRoughness = GetFresnelSchlickRoughness(max(dot(normal, V), 0.0f), F0, roughnessFactor);
    
    kS = flickRoughness;
    kD = 1.0f - kS;
    kD *= 1.0f - metallicFactor;
    

    const float MAX_REFLECTION_LOD = 4.0f;
    //float3 ambient = (kD * Diffuse * diffuseTex.rgb) * 1.0f;
    float3 ambient = (kD * Diffuse * specular) * 1.0f;
    
    float3 output = ambient + Lo;
    output = output / (output + float3(1.0f, 1.0f, 1.0f));
    output = pow(output, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    
    return float4(output, Ambient.a);
}
