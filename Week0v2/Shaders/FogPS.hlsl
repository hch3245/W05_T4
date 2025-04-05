cbuffer FogConstants : register(b0)
{
    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;
    float FogCutoffDistance;
    float FogMaxOpacity;
    float4 FogInscatteringColor;
}

Texture2D SceneColor : register(t0);
Texture2D ScenePosition : register(t1);
SamplerState LinearSampler : register(s0);

float4 main(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_Target
{
    // 월드 위치 읽기
    float3 worldPos = ScenePosition.Sample(LinearSampler, uv).rgb;
    
    // 카메라 기준 거리 계산(StartDistance ~ FogCutoffDistance 까지)
    float distance = length(worldPos);
    
    float fogDistanceFactor = saturate((distance - StartDistance) / (FogCutoffDistance - StartDistance));
    
    
    // 높이에 따른 감쇠 적용
    float fogHeightFactor = exp(-FogHeightFalloff * worldPos.y);
    
    // 최종 안개량
    float fogAmount = saturate(fogHeightFactor * FogDensity * fogDistanceFactor);
    fogAmount = min(fogAmount, FogMaxOpacity);
    
    float3 fogColor = FogInscatteringColor.rgb;
    
    float3 sceneColor = SceneColor.Sample(LinearSampler, uv).rgb;
    float3 finalColor = lerp(sceneColor, fogColor, fogAmount);
    
    return float4(finalColor, 1.0f);
}