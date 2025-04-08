cbuffer FogConstants : register(b0)
{
    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;                        // 안개가 시작되기 시작하는 거리
    float FogCutoffDistance;                    // 안개가 최대치에 도달하는 거리
    float FogMaxOpacity;
    float3 Pad1;
    float4 FogInscatteringColor;
    float3 CameraWorldPos;
    float Pad2;
}

// 다중 Viewport 대응
cbuffer CBViewportParams : register(b1)
{
    float2 ViewportScale;
    float2 ViewportOffset;
}

Texture2D SceneColor : register(t0);
// ScenePosition : 화면상의 픽셀마다 월드좌표가 저장도니 텍스처
Texture2D ScenePosition : register(t1);
// Detph : 깊이 버퍼, 사용 여부에 따라 월드 z로 치환될 수도 있음.
Texture2D Depth : register(t2);
SamplerState LinearSampler : register(s0);

float4 main(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET
{
    float2 adjustedUV = uv * ViewportScale + ViewportOffset;
    
    float3 worldPos = ScenePosition.Sample(LinearSampler, adjustedUV).rgb;
    float depth = Depth.Sample(LinearSampler, adjustedUV).r;
    // 해당 픽셀이 카메라에서 얼마나 떨어졌는지
    // length : 벡터의 유클리디안 거리
    float distance = length(worldPos - CameraWorldPos);
    
    
    // 지금 픽셀이 안개 구간에서 몇% 위치에 있는가를 정규화하는 단계
    // fogDistanceFactor : 0 -> 안개 없음. / 1 -> 안개 최대치 도달.
    // saturate : 결과를 0.0~1.0으로 clamp
    float fogDistanceFactor = saturate((distance - StartDistance) / (FogCutoffDistance - StartDistance));
    if (depth >= 1.0f)
    {
    // 원거리 클리어 값이 들어간 경우
        fogDistanceFactor = 1;
    }
    // 높이에 따른 감쇠 적용
   float fogHeightFactor = exp(-FogHeightFalloff * worldPos.y);
    
    // 최종 안개량
    float fogAmount = saturate(fogHeightFactor * FogDensity * fogDistanceFactor);
    
    
    float3 sceneColor = SceneColor.Sample(LinearSampler, adjustedUV).rgb;
    
    float3 fogColor = pow(FogInscatteringColor.rgb, 2.2); // 감마 → 선형 변환
    
    float3 finalColor = lerp(sceneColor, fogColor, fogAmount);    
    return float4(sceneColor.rgb, 1.0f);
}