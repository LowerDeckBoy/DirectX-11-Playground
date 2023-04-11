
cbuffer AmbientLight
{
    // RGBA
    float4 AmbientColors;
};

struct PS_Input
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(PS_Input pin) : SV_TARGET
{
    float4 output = pin.Color;
    
    return float4(output.xyz, output.a);
}