

cbuffer Material : register(b0)
{
	float4 Ambient;
	float4 Diffuse;
	float3 Direction;
	float Intensity;
	bool bHasNormals;
	bool bHasMetallic;
};


struct PS_Input
{
	float4 Position : SV_POSITION;
	float2 UVs : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
};

Texture2D baseTexture : register(t0);
SamplerState baseSamplerState : register(s0);

Texture2D normalTexture : register(t1);

float4 PS(PS_Input pin) : SV_TARGET
{
	float4 baseColor = baseTexture.Sample(baseSamplerState, pin.UVs);
	
	float4 output = baseColor;
	
	float3 normal    = normalize(pin.Normal);
	float3 tangent   = normalize(pin.Tangent);
	float3 bitangent = normalize(pin.Bitangent);
	
	//https://www.braynzarsoft.net/viewtutorial/q16390-simple-lighting
	if (bHasNormals)
	{
		float4 normalColor = normalTexture.Sample(baseSamplerState, pin.UVs);
		// TODO: check for normalization
        normalColor = normalize(2.0f * normalColor - 1.0f);
		
		tangent = normalize(pin.Tangent - dot(pin.Tangent, pin.Normal) * pin.Normal);
		bitangent = cross(normal, tangent);
		float3x3 texSpace = float3x3(tangent, bitangent, normal);

		normal = normalize(mul(normalColor.xyz, texSpace));
		output += saturate(dot(Direction.xyz, normal) * Ambient * baseColor);
	}
	
	if (bHasMetallic)
	{
		
	}
	
	
	return float4(output.rgb, Ambient.a);
}