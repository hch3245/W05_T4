Texture2D gDepthTexture : register(t0); // t0: 깊이 텍스처 SRV
SamplerState gSampler : register(s0); // s0: 샘플러

cbuffer CBCameraNearFar : register(b0)
{
    float NearPlane;
    float FarPlane;
}

cbuffer CBViewportParams : register(b1)
{
    float2 ViewportScale;
    float2 ViewportOffset;
}

struct VS_INPUT
{
    float3 Pos : POSITION; // 정점 위치 (-1~1 범위 가정)
    float2 Tex : TEXCOORD0; // UV 좌표 (0~1 범위 가정)
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION; // 클립 공간 위치
    float2 Tex : TEXCOORD0; // UV 좌표
};

// Vertex Shader: 입력 정점을 그대로 전달하고 UV 생성
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    // 입력 Pos는 이미 -1~1 범위의 클립 공간 XY 좌표로 가정
    // Z는 0으로, W는 1로 설정하여 화면에 꽉 차게 함
    output.Pos = float4(input.Pos.xy, 0.0f, 1.0f);
    // 입력 UV 좌표 그대로 전달
    output.Tex = input.Tex;
    return output;
}

// Pixel Shader: 깊이 텍스처 샘플링 후 회색조 출력
// 픽셀 셰이더 (수정됨)
float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // 1. 뷰포트 UV를 전체 텍스처 UV로 조정
    //    input.Tex는 현재 뷰포트 내에서의 상대 좌표 (0~1)
    //    adjustedUV는 전체 깊이 텍스처에서의 절대 좌표 (0~1)
    float2 adjustedUV = input.Tex * ViewportScale + ViewportOffset;

    // 2. 조정된 UV로 깊이 텍스처 샘플링
    //    depthValue는 0.0 (Near) ~ 1.0 (Far) 범위의 비선형 값
    float depthValue = gDepthTexture.Sample(gSampler, adjustedUV).r;

    // 3. 비선형 깊이를 선형 깊이(실제 거리)로 변환
    float linearDepth = NearPlane * FarPlane / (FarPlane - depthValue * (FarPlane - NearPlane));

    // 4. 선형 깊이를 시각화 범위에 맞게 정규화 (0~1)
    float normalizedDepth = saturate(linearDepth / FarPlane);

    // --- 범위 재매핑 적용 ---
    float minVisibleNormDepth = 0.0f; // 실제로 보이는 가장 가까운 값의 정규화된 값 (예상치 또는 계산값)
    float maxVisibleNormDepth = 0.2f; // 실제로 보이는 가장 먼 값의 정규화된 값 (예상치 또는 계산값, 예: 0.2는 MaxDistance의 20%)
                                      // 이 값들을 조정하여 대비 조절
    
    

    // 0으로 나누기 방지
    float range = max(maxVisibleNormDepth - minVisibleNormDepth, 0.00001f);
    float remappedDepth = saturate((normalizedDepth - minVisibleNormDepth) / range);

    return float4(remappedDepth, remappedDepth, remappedDepth, 1.0f);

    //    만약 반대로 가까울수록 밝게(1), 멀수록 어둡게(0) 하고 싶다면:
    //    return float4(1.0f - normalizedDepth, 1.0f - normalizedDepth, 1.0f - normalizedDepth, 1.0f);
}