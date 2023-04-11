
cbuffer Light : register(b1)
{
    float4 Ambient;
    float3 Diffuse;
    float padding;
    float3 Position;
    float padding2;
    float Distance;
    float Intensity;
    float2 padding3;
    float Attenuation_A;
    float Attenuation_B;
    float Attenuation_C;
   
};

struct PS_Input
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : WORLD_POS;
    float2 UVs : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

Texture2D baseTexture : register(t0);
SamplerState baseSamplerState : register(s0);

//https://www.braynzarsoft.net/viewtutorial/q16390-17-point-lights
float4 PS(PS_Input pin) : SV_TARGET
{
    pin.Normal = normalize(pin.Normal);

    float4 diffuse = baseTexture.Sample(baseSamplerState, pin.UVs);
    float3 ambient = Ambient.xyz * Ambient.a;
    
    float3 lightVector = normalize(Position - pin.WorldPosition.xyz);
    float distanceToLight = length(lightVector);

    float3 output = ambient;
    
    // normalize light
    //lightVector /= distanceToLight;
    float3 lightDirection = lightVector / distanceToLight;
    
    float lightIntensity = max(dot(lightDirection, pin.Normal), 0.0f);
    //float lightIntensity = max(dot(lightDirection, pin.WorldPosition.xyz), 0.0f);

    
    float attenuation = 1.0f / (Attenuation_A + (Attenuation_B * distanceToLight) + (Attenuation_C * (distanceToLight * distanceToLight)));
    // * Diffuse
    //output += diffuse.rgb * attenuation * lightIntensity;
    //output += ambient.rgb * attenuation * (lightIntensity * Diffuse);
    //output = lightIntensity * attenuation * Distance * Diffuse;
    //* Distance
    float3 dif = lightIntensity * Diffuse;
    output += dif;
    //float3 output = ambient;
    //output = diffuse.rgb * output;
    
    //output = saturate(output + ambient);
    //output = saturate(output);
    //ambient += output;
    float3 final = diffuse.rgb * output;
    
    //return float4(output.rgb, Ambient.a);
    return float4(final.rgb, Ambient.a);
}