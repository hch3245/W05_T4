Texture2D Textures : register(t0);
SamplerState Sampler : register(s0);

cbuffer MatrixConstants : register(b0)
{
    row_major float4x4 MVP;
    row_major float4x4 M;
    row_major float4x4 MInverseTranspose;
    float4 UUID;
    bool isSelected;
    float3 MatrixPad0;
};

struct FMaterial
{
    float3 DiffuseColor;
    float TransparencyScalar;
    float3 AmbientColor;
    float DensityScalar;
    float3 SpecularColor;
    float SpecularScalar;
    float3 EmissiveColor;
    float MaterialPad0;
};

cbuffer MaterialConstants : register(b1)
{
    FMaterial Material;
}

cbuffer LightingConstants : register(b2)
{
    float3 LightDirection; // 조명 방향 (단위 벡터; 빛이 들어오는 방향의 반대 사용)
    float LightPad0; // 16바이트 정렬용 패딩
    float3 LightColor; // 조명 색상 (예: (1, 1, 1))
    float LightPad1; // 16바이트 정렬용 패딩
    float AmbientFactor; // ambient 계수 (예: 0.1)
    float3 LightPad2; // 16바이트 정렬 맞춤 추가 패딩
};

cbuffer FlagConstants : register(b3)
{
    bool IsLit;
    float3 flagPad0;
}

cbuffer SubMeshConstants : register(b4)
{
    bool IsSelectedSubMesh;
    float3 SubMeshPad0;
}

cbuffer TextureConstants : register(b5)
{
    float2 UVOffset;
    float2 TexturePad0;
}

struct FLightConstants
{
    float4 Color;
    
    float3 Position;
    float Radius;

    float3 Direction;
    float SpotAngle;

    float Intensity;
    int Type;
    float pad0;
    float pad1;
};

StructuredBuffer<FLightConstants> g_Lights : register(t5);
cbuffer FrameConstants : register(b6)
{
    int LightCount;
}

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float4 color : COLOR; // 전달할 색상
    float3 normal : NORMAL; // 정규화된 노멀 벡터
    bool normalFlag : TEXCOORD0; // 노멀 유효성 플래그 (1.0: 유효, 0.0: 무효)
    float2 texcoord : TEXCOORD1;
    int materialIndex : MATERIAL_INDEX;
    float4 worldPosition : POSITION;
};

struct PS_OUTPUT
{
    float4 color : SV_Target0;
    float4 UUID : SV_Target1;
    float4 WorldPos : SV_Target2;  // 월드 좌표계임
};

float noise(float3 p)
{
    return frac(sin(dot(p, float3(12.9898, 78.233, 37.719))) * 43758.5453);
}

float4 PaperTexture(float3 originalColor)
{
    // 입력 색상을 0~1 범위로 제한
    float3 color = saturate(originalColor);
    
    float3 paperColor = float3(0.95, 0.95, 0.95);
    float blendFactor = 0.5;
    float3 mixedColor = lerp(color, paperColor, blendFactor);
    
    // 정적 grain 효과
    float grain = noise(color * 10.0) * 0.1;
    
    // 거친 질감 효과: 두 단계의 노이즈 레이어를 결합
    float rough1 = (noise(color * 20.0) - 0.5) * 0.15; // 첫 번째 레이어: 큰 규모의 노이즈
    float rough2 = (noise(color * 40.0) - 0.5) * 0.01; // 두 번째 레이어: 세부 질감 추가
    float rough = rough1 + rough2;
    
    // vignette 효과: 중앙에서 멀어질수록 어두워지는 효과
    float vignetting = smoothstep(0.4, 1.0, length(color.xy - 0.5) * 2.0);
    
    // 최종 색상 계산
    float3 finalColor = mixedColor + grain + rough - vignetting * 0.1;
    return float4(saturate(finalColor), 1.0);
}

PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;
    
    output.UUID = UUID;
    
    output.WorldPos = input.worldPosition;
    
    float3 texColor = Textures.Sample(Sampler, input.texcoord + UVOffset);
    float3 color;
    if (texColor.g == 0) // TODO: boolean으로 변경
        color = saturate(Material.DiffuseColor);
    else
    {
        color = texColor + Material.DiffuseColor;
    }
    
    if (isSelected)
    {
        color += float3(0.2f, 0.2f, 0.0f); // 노란색 틴트로 하이라이트
        if (IsSelectedSubMesh)
            color = float3(1, 1, 1);
    }
    
    // 발광 색상 추가

    if (IsLit == 1)
    {
        if (input.normalFlag > 0.5)
        {
            float3 N = normalize(input.normal);
            float3 V = float3(0, 0, 1); // 카메라는 +Z를 바라본다고 가정
            float3 litDiffuse = float3(0, 0, 0);
            float3 litSpecular = float3(0, 0, 0);

            for (int i = 0; i < LightCount; ++i)
            {
                FLightConstants light = g_Lights[i];

                float3 L = float3(0, 0, 0);
                float attenuation = 1.0f;

                if (light.Type == 0) // Point Light
                {
                    float3 toLight = light.Position - input.worldPosition.xyz;
                    float dist = length(toLight);
                    L = normalize(toLight);
                    attenuation = saturate(1.0 - dist / light.Radius);
                }
                else if (light.Type == 1) // Directional Light
                {
                    L = normalize(-light.Direction);
                }
                else if (light.Type == 2) // Spot Light
                {
                    float3 toLight = light.Position - input.worldPosition.xyz;
                    float dist = length(toLight);
                    L = normalize(toLight);
                    float spotFactor = saturate(dot(-L, normalize(light.Direction)));
                    float cone = cos(light.SpotAngle);
                    if (spotFactor < cone)
                        continue;
                    float falloff = smoothstep(cone, 1.0, spotFactor);
                    attenuation = falloff * saturate(1.0 - dist / light.Radius);
                }

            // 디퓨즈 조명
                float NdotL = saturate(dot(N, L));
                float3 diffuse = NdotL * light.Color.rgb * light.Intensity;

            // 스페큘러 조명 (Blinn-Phong)
                float3 H = normalize(L + V);
                float spec = pow(saturate(dot(N, H)), Material.SpecularScalar * 32);
                float3 specular = spec * Material.SpecularColor * light.Color.rgb * light.Intensity;

            // 감쇠 적용
                litDiffuse += diffuse * attenuation;
                litSpecular += specular * attenuation;
            }

        // Ambient
            float3 ambient = Material.AmbientColor * AmbientFactor;

        // 최종 조명 조합
            color = ambient + (litDiffuse * color) + litSpecular;
        }

    // 발광 색상 + 투명도
        color += Material.EmissiveColor;
        output.color = float4(color, Material.TransparencyScalar);
        return output;
    }
    else // unlit 상태일 때 PaperTexture 효과 적용
    {
        if (input.normalFlag < 0.5)
        {
            output.color = float4(color, Material.TransparencyScalar);
            return output;
        }
        
        output.color = float4(color, 1);
        // 투명도 적용
        output.color.a = Material.TransparencyScalar;
            
        return output;
    }
}