cbuffer FogConstants : register(b0)
{
    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;                        // 안개가 시작되기 시작하는 거리
    float FogCutoffDistance;                    // 안개가 최대치에 도달하는 거리
    float FogMaxOpacity;
    float FarClip;
    float2 Pad1;
    float4 FogInscatteringColor;
    float3 CameraWorldPos;
    float Pad2;
    float4x4 InvProjection;
    
}
Texture2D SceneColor : register(t0);
// ScenePosition : 화면상의 픽셀마다 월드좌표가 저장도니 텍스처
Texture2D ScenePosition : register(t1);
// Detph : 깊이 버퍼, 사용 여부에 따라 월드 z로 치환될 수도 있음.
Texture2D Depth : register(t2);
SamplerState LinearSampler : register(s0);

float4 main(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET
{
    //float3 worldPos = ScenePosition.Sample(LinearSampler, uv).rgb;
    float3 worldPos = ScenePosition.Sample(LinearSampler, uv).rgb;
    float depth = Depth.Sample(LinearSampler, uv).r;
    // 해당 픽셀이 카메라에서 얼마나 떨어졌는지
    // length : 벡터의 유클리디안 거리
    float distance = length(worldPos - CameraWorldPos);
    
    
    // 지금 픽셀이 안개 구간에서 몇% 위치에 있는가를 정규화하는 단계
    // fogDistanceFactor : 0 -> 안개 없음. / 1 -> 안개 최대치 도달.
    // saturate : 결과를 0.0~1.0으로 clamp
    //float fogDistanceFactor = saturate((distance - StartDistance) / (FogCutoffDistance - StartDistance));
    //float fogDistanceFactor = 1.0 - exp(-distance * FogDensity);
    
    float rawFog = 1.0 - exp(-distance * FogDensity);
    float fogDistanceFactor = saturate((rawFog - StartDistance) / (FogCutoffDistance - StartDistance));
    
    //if (depth >= 1.0f)
    //{
    //    float2 ndc = uv * 2.0 - 1.0;
    //    ndc.y = -ndc.y;
        
    //    float4 clipPos = float4(ndc.x, ndc.y, 1.0f, 1.0f);
    //    float4 viewPos = mul(clipPos, InvProjection);
        
    //    float3 viewDir = normalize(viewPos.xyz / viewPos.w);
        
    //    worldPos = CameraWorldPos + viewDir * FarClip;
    //}
    // 높이에 따른 감쇠 적용
   //float fogHeightFactor = exp(-FogHeightFalloff * worldPos.y);
    float fogHeightFactor = exp(-FogHeightFalloff * worldPos.z);
    // 최종 안개량
    //float fogAmount = saturate(fogHeightFactor * FogDensity * fogDistanceFactor);
    float fogAmount = saturate(fogDistanceFactor * fogHeightFactor);
    fogAmount = pow(fogAmount, 1.2);
    float3 sceneColor = SceneColor.Sample(LinearSampler, uv).rgb;
    
    float3 fogColor = pow(FogInscatteringColor.rgb, 2.2); // 감마 -> 선형 변환
    
    float3 finalColor = lerp(sceneColor, fogColor, fogAmount);    
    return float4(finalColor.rgb, 1.0f);
}