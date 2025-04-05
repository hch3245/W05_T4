Texture2D gDepthTexture : register(t0); // t0: 깊이 텍스처 SRV
SamplerState gSampler : register(s0); // s0: 샘플러

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
float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float depthValue = gDepthTexture.Sample(gSampler, input.Tex).r;

    // 예시: 0.95 ~ 1.0 범위를 0.0 ~ 1.0 으로 확장 (값은 조정 필요)
    float minVisibleDepth = 0.95f; // 씬에 따라 조절해야 하는 값
    float remappedDepth = saturate((depthValue - minVisibleDepth) / (1.0f - minVisibleDepth));

    return float4(remappedDepth, remappedDepth, remappedDepth, 1.0f);
}