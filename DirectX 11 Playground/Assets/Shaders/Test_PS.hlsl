// https://www.3dgep.com/texturing-lighting-directx-11/#Material_Properties
// https://stackoverflow.com/questions/34527505/blinn-phong-shading-with-hlsl-and-d3d11-point-light
// https://pl.wikipedia.org/wiki/Cieniowanie_Phonga

cbuffer Material : register(b0)
{
    float4 Albedo;
    float MetallicFactor;
    float RoughnessFactor;
    float2 padding;
    float3 Camera;
    bool bHasNormals;
    bool bHasMetallic;
};

struct PS_Input
{
    float4 Position : SV_POSITION;
    float4 WorldPos : WORLD_POS;
    float2 UVs : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

Texture2D baseTexture : register(t0);
SamplerState baseSamplerState : register(s0);

Texture2D normalTexture : register(t1);
Texture2D metallicTexture : register(t2);

void Compute(PS_Input pin, float3 viewDir, float3 normalVec, float3 lightPos, float3 lightColor, float F0, out float3 rad)
{
    const float PI = 3.14159265359f;
    
    //calculate light radiance
    float3 lightDir = normalize(lightPos - pin.WorldPos.xyz);
    float3 halfwayVec = normalize(viewDir + lightDir);
    float distance = length(lightPos - pin.WorldPos.xyz);
    float attenuation = 1.0f / dot(float3(1.0f, 0.0f, 1.0f), float3(1.0f, distance, distance * distance));
    float3 radiance = lightColor * attenuation;

	//Cook-Torrance BRDF
    float D = NormalDistributionGGXTR(normalVec, halfwayVec, RoughnessFactor);
    float G = GeometrySmith(normalVec, viewDir, lightDir, RoughnessFactor);
    float3 F = FresnelSchlick(max(dot(halfwayVec, viewDir), 0.0f), F0);

    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= (1.0 - metallic);

    float3 nom = D * G * F;
    float denom = 4 * max(dot(normalVec, viewDir), 0.0f) * max(dot(normalVec, lightDir), 0.0) + 0.001f; // 0.001f just in case product is 0
    float3 specular = nom / denom;

	//Add to outgoing radiance Lo
    float NdotL = max(dot(normalVec, lightDir), 0.0f);
    rad = (((kD * albedo / PI) + specular) * radiance * NdotL);
    
}


float4 PS(PS_Input pin) : SV_TARGET
{
    float4 baseOutput = baseTexture.Sample(baseSamplerState, pin.UVs);

    float3 normal = normalize(pin.Normal);
    float3 tangent = normalize(pin.Tangent);
    float3 bitangent = normalize(pin.Bitangent);
    
    float3 output = baseOutput.xyz;
        
    if (bHasNormals)
    {
        float4 normalOutput = normalTexture.Sample(baseSamplerState, pin.UVs);
        normalOutput = normalize(2.0 * normalOutput - 1.0);
        
        tangent = normalize(pin.Tangent - dot(pin.Tangent, pin.Normal) * pin.Normal);
        float3 biTangent = cross(pin.Normal, pin.Tangent);
        float3x3 texSpace = float3x3(pin.Tangent, biTangent, pin.Normal);

        //Convert normal from normal map to texture space and store in input.normal
        normal = normalize(mul(normalOutput.xyz, texSpace));
        output += saturate(dot(Camera.xyz, normal) * Albedo * baseOutput);
    }
    
    if (bHasMetallic)
    {
        float4 metallicOutput = metallicTexture.Sample(baseSamplerState, pin.UVs);
        float roughness = RoughnessFactor;
        roughness = metallicOutput.g * roughness;
        
        float metallic = MetallicFactor;
        metallic = metallicOutput.b * metallic;
        
        roughness = clamp(roughness, 0.0f, 1.0f);
        metallic = clamp(metallic, 0.0f, 1.0f);
        
        float3 f0 = float3(0.04f, 0.04f, 0.04f);
        float3 diffuse = baseOutput.rgb * (float3(1.0f, 1.0f, 1.0f) - f0);
        diffuse *= 1.0f - metallic;
        
        float3 specular = lerp(f0, baseOutput.rgb, metallic);
        //specularMultiplier = (1.0f - roughnessOutput);
        //SpecularIntensity = lerp(1.0f, 128.0f, pow(1.0f - roughnessOutput, 3.0));
        output += lerp(output, specular, 1.0f);
        
        
        //https://github.com/TheEvilBanana/PhysicallyBasedRendering/blob/master/PBRMatPixelShader.hlsl
        float3 normalVector = pin.Normal;
        float3 viewDirection = normalize(Camera.xyz - pin.WorldPos.xyz);
        float3 reflection = reflect(-viewDirection, normalVector);
        float3 F0 = float3(0.04f, 0.04f, 0.04f);
        F0 = lerp(F0, Albedo.rgb, metallic);

        float3 rad = float3(0.0f, 0.0f, 0.0f);
	    //reflectance equation
        float3 Lo = float3(0.0f, 0.0f, 0.0f);   
        //output *= F0;
    }
    
    
    
    
    
    return float4(output, Albedo.a);
}