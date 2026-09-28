#include "PixelShaderBase.hlsli"

cbuffer OutLineShaderParam : register(b4)
{
    float4 GlowColor;   // 色
    float OutlineWidth; // 輪郭 (px)
    float Time;         // 時間経過 (sec)
    float2 TextureSize; // テクスチャの解像度(w, h)
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = Texture0.Sample(Sampler0, input.UV);
    float2 texelSize = (1.0f / TextureSize) * OutlineWidth;
    float alpha = -4.0 * color.a;
    
    alpha += Texture0.Sample(Sampler0, input.UV + float2(texelSize.x, 0.0)).a;
    alpha += Texture0.Sample(Sampler0, input.UV + float2(-texelSize.x, 0.0)).a;
    alpha += Texture0.Sample(Sampler0, input.UV + float2(0.0, texelSize.y)).a;
    alpha += Texture0.Sample(Sampler0, input.UV + float2(0.0, -texelSize.y)).a;

    float blend_a = saturate(alpha);
    float outline_a = abs(alpha);
    float blink = sin(Time * 3.14159) * 0.5 + 0.5;
    float glow_strength = lerp(0.3, 1.0, blink);
    float4 final_color = lerp(
        color,
        GlowColor,
        blend_a * glow_strength
    );
    return float4(final_color.rgb, saturate(color.a + outline_a));
}