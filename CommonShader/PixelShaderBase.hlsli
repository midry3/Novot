#pragma once

Texture2D Texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Diffuse : COLOR0;
    float4 Specular : COLOR1;
    float2 UV : TEXCOORD0;
    float2 SubUV : TEXCOORD1;
};