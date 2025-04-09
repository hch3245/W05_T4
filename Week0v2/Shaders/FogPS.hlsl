cbuffer FogConstants : register(b0)
{
    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;                        // 안개가 시작되기 시작하는 거리
    float FogCutoffDistance;                    // 안개가 최대치에 도달하는 거리
    float FogMaxOpacity;
    float FarClip;
    float FogBaseHeight;
    float Pad1;
    float4 FogInscatteringColor;
    float3 CameraWorldPos;
    float Pad2;
    float4x4 InvProjection;
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
    float2 adjustUV = uv * ViewportScale + ViewportOffset;
    
    // 픽셀의 월드 좌표와 카메라 간의 거리 계산
    float3 worldPos = ScenePosition.Sample(LinearSampler, adjustUV).rgb;
    float3 viewDir = worldPos - CameraWorldPos;
    float distance = length(viewDir);

    // -----------------------------------------------------
    // 1. 거리 기반 안개 계산 (Exponential)
    //    - StartDistance 이후부터 안개 적용 (StartDistance 이하이면 0)
    //    - distanceTerm은 안개가 시작되는 지점 이후의 거리
    float distanceTerm = max(0.0, distance - StartDistance);
    //    - 지수 함수로 안개 누적: (값이 작을수록 안개가 덜 끼고, 멀어질수록 1에 가까워짐)
    float distanceFogFactor = 1.0 - exp(-FogDensity * distanceTerm);
    distanceFogFactor = saturate(distanceFogFactor); // [0,1]로 클램프

    // -----------------------------------------------------
    // 2. 높이 기반 안개 계산 (Exponential)
    //   FogBaseHeight 기준으로 아래로 갈수록 안개가 진해짐. 
    //float heightTerm = max(0.0, -worldPos.z);
    float heightTerm = FogBaseHeight - worldPos.z;
    heightTerm = max(0.0, heightTerm);
    float heightFogFactor = 1.0 - exp(-FogHeightFalloff * heightTerm);
    heightFogFactor = saturate(heightFogFactor);

    // -----------------------------------------------------
    // 3. 최종 안개 양 결정
    //    - 거리와 높이의 효과를 곱해서 최종 안개 양 결정
    float fogAmount = distanceFogFactor * heightFogFactor;
    //    - FogMaxOpacity를 넘지 않도록 제한 (에셋이나 설정에 따라 조절)
    fogAmount = min(fogAmount, FogMaxOpacity);
    
    // -----------------------------------------------------
    // 4. 색상 보간 및 최종 색상 계산
    //    - FogInscatteringColor는 감마 보정 후 선형 공간으로 변환
    float3 fogColor = pow(FogInscatteringColor.rgb, 2.2);
    float3 sceneColor = SceneColor.Sample(LinearSampler, adjustUV).rgb;
    
    //    - 원래 장면 색과 안개 색을 fogAmount만큼 보간
    float3 finalColor = lerp(sceneColor, fogColor, fogAmount);
    
    return float4(finalColor, 1.0);
}