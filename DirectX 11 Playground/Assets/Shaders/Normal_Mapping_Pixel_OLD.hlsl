
cbuffer Material : register(b0)
{
    float4 Ambient;
    float3 Diffuse;
    float pad;
    float3 Direction;
    float pad2;
    bool bHasDiffuse;
    bool bHasNormals;
    bool bHasMetallic;
    float Intensity;
};

struct PS_Input
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : WORLD_POS;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

Texture2D baseTexture : register(t0);
SamplerState baseSamplerState : register(s0);

Texture2D normalTexture : register(t1);


float4 PS(PS_Input pin) : SV_TARGET
{
    pin.Normal = normalize(pin.Normal);
    
    if (!bHasDiffuse && !bHasNormals && !bHasMetallic)
    {
        return float4(Ambient.xyz, Ambient.a);
    }
    
    float4 baseOutput = float4(0.0f, 0.0f, 0.0f, 0.0f);

    if (bHasDiffuse == true)
    {
        baseOutput = baseTexture.Sample(baseSamplerState, pin.UV);

    }
    //float3 ambient = Ambient.xyz;
    float3 ambient = baseOutput.rgb * Ambient.xyz;
    
    float3 colorOutput = ambient.xyz;
   //float3 ambient = baseOutput.rgb * Ambient.xyz;
    
    //float3 colorOutput = float3(1.0f, 1.0f, 1.0f);
    //colorOutput = ambient.rgb;
    
    if (bHasNormals == true)
    {
        //colorOutput = ambient.rgb;
        
        float4 normalOutput = normalTexture.Sample(baseSamplerState, pin.UV);
        normalOutput = (2.0f * normalOutput) - 1.0f;
        

        float3 tangent = normalize(pin.Tangent - dot(pin.Tangent, pin.Normal) * pin.Normal);
        //float3 bitangent = cross(pin.Normal, pin.Tangent);
        float3 bitangent = cross(pin.Normal, tangent);
        float3x3 texSpace = float3x3(tangent, bitangent, pin.Normal);
        
        pin.Normal = normalize(mul(normalOutput.xyz, texSpace));
        
        //test
        float3 bumpNormal = (normalOutput.x * pin.Tangent) + (normalOutput.y * pin.Bitangent) + (normalOutput.z * pin.Normal);
        bumpNormal = normalize(bumpNormal);
        float3 lightDirection = Direction.xyz - pin.WorldPosition.xyz;
        float intensity = saturate(dot(bumpNormal, lightDirection));
        
        //colorOutput += saturate(dot(Direction.xyz, inputData.Normal) * (colorOutput + ambient));
        //colorOutput += saturate(dot(Direction.xyz, pin.Normal) * Diffuse.xyz * ambient.rgb);
        colorOutput += saturate(Diffuse.xyz * intensity);
        colorOutput = colorOutput * ambient.rgb;

    }
    
    return float4(colorOutput.rgb, Ambient.a);
}