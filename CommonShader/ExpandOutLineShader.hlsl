#include "PixelShaderBase.hlsli"

cbuffer ExpandOutLineShaderParam : register(b4)
{
    float4 GlowColor; // アウトライン色
    float OutlineWidth; // アウトラインの幅 (px)
    float Time; // 経過時間 (sec)
    float ExpandSpeed; // 拡大速度 (px/sec)
    float LoopInterval; // 1回拡大し終わってから次までの待機時間 (sec)
    float2 TextureSize; // テクスチャ解像度 (w, h)
    float2 Padding;
};

#define PI 3.14159265359f
#define SAMPLE_COUNT 8
#define DISTANCE_SAMPLES 16

float SampleAlpha(float2 uv)
{
    // テクスチャ外は透明として扱う
    if (uv.x < 0.0f || uv.x > 1.0f ||
        uv.y < 0.0f || uv.y > 1.0f)
    {
        return 0.0f;
    }

    return Texture0.Sample(Sampler0, uv).a;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = Texture0.Sample(Sampler0, input.UV);
    color.rgb *= input.Diffuse.rgb;

    // ------------------------------------------------------------
    // アニメーション時間
    // ------------------------------------------------------------

    float maxDistance = length(TextureSize * 0.5f);

    float expandDuration =
        maxDistance / max(ExpandSpeed, 0.0001f);

    float cycleDuration =
        expandDuration + LoopInterval;

    float currentCycleTime =
        fmod(Time, cycleDuration);

    // 待機中
    if (currentCycleTime >= expandDuration)
    {
        return color;
    }

    // 現在の波の半径
    float radius =
        currentCycleTime * ExpandSpeed;


    // ------------------------------------------------------------
    // アウトラインのリング
    //
    //     innerRadius
    //        ↓
    //      ┌─────┐
    //      │#####│
    //      │#####│
    //      └─────┘
    //        ↑
    //     outerRadius
    //
    // 幅は常に OutlineWidth
    // ------------------------------------------------------------

    float innerRadius = max(radius - OutlineWidth * 0.5f, 0.0f);
    float outerRadius = radius + OutlineWidth * 0.5f;


    // ------------------------------------------------------------
    // 16方向からアルファを探索
    // ------------------------------------------------------------

    float nearestDistance = maxDistance;
    float2 texelSize = 1.0f / TextureSize;

    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        float angle =
        (float(i) / float(SAMPLE_COUNT)) * PI * 2.0f;

        float2 direction =
        float2(cos(angle), sin(angle));

    [unroll]
        for (int j = 0; j < DISTANCE_SAMPLES; ++j)
        {
            float d =
            maxDistance *
            (float(j) / float(DISTANCE_SAMPLES - 1));

            float2 sampleUV =
            input.UV + direction * texelSize * d;

            float alpha = SampleAlpha(sampleUV);

            if (alpha > 0.5f)
            {
                nearestDistance = min(nearestDistance, d);
                break;
            }
        }
    }


    // ------------------------------------------------------------
    // 波の位置とエッジの距離を比較
    // ------------------------------------------------------------

    float distanceFromEdge =
        abs(nearestDistance - radius);

    /*
     * エッジから OutlineWidth/2 の範囲を
     * アウトラインとして扱う。
     */
    float outlineAlpha =
        1.0f - smoothstep(
            0.0f,
            OutlineWidth * 0.5f,
            distanceFromEdge
        );


    // ------------------------------------------------------------
    // 元画像の内側にはアウトラインを出さない
    // ------------------------------------------------------------

    float centerAlpha = SampleAlpha(input.UV);

    outlineAlpha *= 1.0f - centerAlpha;


    // ------------------------------------------------------------
    // テクスチャ端でフェード
    // ------------------------------------------------------------

    float2 edgeDistance =
        min(input.UV, 1.0f - input.UV);

    float edgeFade =
        saturate(
            min(edgeDistance.x, edgeDistance.y)
            * min(TextureSize.x, TextureSize.y)
            / max(OutlineWidth, 1.0f)
        );

    outlineAlpha *= edgeFade;


    // ------------------------------------------------------------
    // 拡大終端でフェード
    // ------------------------------------------------------------

    float distanceFade =
        saturate(
            1.0f -
            radius / maxDistance
        );

    outlineAlpha *= distanceFade;


    // ------------------------------------------------------------
    // 色を合成
    // ------------------------------------------------------------

    float finalOutlineAlpha =
        saturate(outlineAlpha * GlowColor.a);

    float3 finalRGB =
        lerp(
            color.rgb,
            GlowColor.rgb,
            finalOutlineAlpha
        );

    float finalA =
        saturate(
            color.a +
            finalOutlineAlpha
        );

    return float4(finalRGB, finalA);
}