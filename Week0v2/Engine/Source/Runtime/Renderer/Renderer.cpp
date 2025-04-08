#include "Renderer.h"
#include <d3dcompiler.h>

#include "Engine/World.h"
#include "Actors/Player.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/UBillboardComponent.h"
#include "Components/UParticleSubUVComp.h"
#include "Components/UText.h"
#include "Components/Material/Material.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Launch/EditorEngine.h"
#include "Math/JungleMath.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "UObject/Casts.h"
#include "UObject/Object.h"
#include "PropertyEditor/ShowFlags.h"
#include "UObject/UObjectIterator.h"
#include "Components/SkySphereComponent.h"
#include "Runtime/Launch/Define.h"


void FRenderer::Initialize(FGraphicsDevice* graphics)
{
    Graphics = graphics;
    RenderResourceManager.Initialize(Graphics->Device);
    ShaderManager.Initialize(Graphics->Device, Graphics->DeviceContext);
    ConstantBufferUpdater.Initialize(Graphics->DeviceContext);

    CreateShader();
    CreateConstantBuffer();
    CreateDepthVisualizationResources(); // 깊이 시각화 리소스 생성
    CreateFogResources();
    ConstantBufferUpdater.UpdateLitUnlitConstant(FlagBuffer, 1);

}

void FRenderer::Release()
{
    ReleaseShader();
    ReleaseConstantBuffer();
    ReleaseDepthVisualizationResources(); // 깊이 시각화 리소스 해제
    ReleaseFogResources();
}

#pragma region Shader
void FRenderer::CreateShader()
{
    // 기본 셰이더 설정
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"MATERIAL_INDEX", 0, DXGI_FORMAT_R32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    ShaderManager.CreateVertexShader(
        L"Shaders/StaticMeshVertexShader.hlsl", "mainVS",
        VertexShader, layout, ARRAYSIZE(layout), &InputLayout, &Stride, sizeof(FVertexSimple));

    ShaderManager.CreatePixelShader(
        L"Shaders/StaticMeshPixelShader.hlsl", "mainPS",
        PixelShader);

    // 텍스쳐 셰이더 설정
    D3D11_INPUT_ELEMENT_DESC textureLayout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    ShaderManager.CreateVertexShader(
        L"Shaders/VertexTextureShader.hlsl", "main",
        VertexTextureShader, textureLayout, ARRAYSIZE(textureLayout), &TextureInputLayout, &TextureStride, sizeof(FVertexTexture));

    ShaderManager.CreatePixelShader(
        L"Shaders/PixelTextureShader.hlsl", "main",
        PixelTextureShader);

    // 라인 셰이더 설정
    ShaderManager.CreateVertexShader(
        L"Shaders/ShaderLine.hlsl", "mainVS",
        VertexLineShader, nullptr, 0); // 라인 셰이더는 Layout 안 쓰면 nullptr 전달

    ShaderManager.CreatePixelShader(
        L"Shaders/ShaderLine.hlsl", "mainPS",
        PixelLineShader);
}

void FRenderer::ReleaseShader()
{
    ShaderManager.ReleaseShader(InputLayout, VertexShader, PixelShader);
    ShaderManager.ReleaseShader(TextureInputLayout, VertexTextureShader, PixelTextureShader);
    ShaderManager.ReleaseShader(nullptr, VertexLineShader, PixelLineShader);
}


// Prepare
void FRenderer::PrepareShader() const
{
    // 정점 셰이더 바인딩.
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    // 픽셀 셰이더 바인딩.
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    // Vertex Buffer 데이터 포맷에 맞는 Input Layout 설정.
    Graphics->DeviceContext->IASetInputLayout(InputLayout);

    if (ConstantBuffer)
    {
        // b0 slot 바인딩
        Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &ConstantBuffer);
        
        // 머티리얼 Cbuffer 픽셀 셰이더 b1 slot 바인딩
        Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &MaterialConstantBuffer);
        // 라이트 Cbuffer 픽셀 세이더 b2 slot 바인딩
        Graphics->DeviceContext->PSSetConstantBuffers(2, 1, &LightingBuffer);
        // Lit/Unlit flag 관련 b3 slot 바인딩
        Graphics->DeviceContext->PSSetConstantBuffers(3, 1, &FlagBuffer);
        // 서브 메시 선택 여부 Cbuffer b3 바인딩.
        Graphics->DeviceContext->PSSetConstantBuffers(4, 1, &SubMeshConstantBuffer);
        // 텍스처 Cbuffer  b5 slot 바인딩.
        Graphics->DeviceContext->PSSetConstantBuffers(5, 1, &TextureConstantBufer);
    }
}

void FRenderer::PrepareTextureShader() const
{
    Graphics->DeviceContext->VSSetShader(VertexTextureShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelTextureShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(TextureInputLayout);

    if (ConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
    }
}

void FRenderer::PrepareSubUVConstant() const
{
    if (SubUVConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(1, 1, &SubUVConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &SubUVConstantBuffer);
    }
}

void FRenderer::PrepareLineShader() const
{
    Graphics->DeviceContext->VSSetShader(VertexLineShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelLineShader, nullptr, 0);

    if (ConstantBuffer && GridConstantBuffer)
    {
        Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);     // MatrixBuffer (b0)
        Graphics->DeviceContext->VSSetConstantBuffers(1, 1, &GridConstantBuffer); // GridParameters (b1)
        Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &GridConstantBuffer);
        Graphics->DeviceContext->VSSetConstantBuffers(3, 1, &LinePrimitiveBuffer);
        Graphics->DeviceContext->VSSetShaderResources(2, 1, &pBBSRV);
        Graphics->DeviceContext->VSSetShaderResources(3, 1, &pConeSRV);
        Graphics->DeviceContext->VSSetShaderResources(4, 1, &pOBBSRV);
    }
}
#pragma endregion Shader


#pragma region ConstantBuffer
// ConstantBuffer
void FRenderer::CreateConstantBuffer()
{
    ConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FConstants));
    SubUVConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FSubUVConstant));
    GridConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FGridParameters));
    LinePrimitiveBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FPrimitiveCounts));
    MaterialConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FMaterialConstants));
    SubMeshConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FSubMeshConstants));
    TextureConstantBufer = RenderResourceManager.CreateConstantBuffer(sizeof(FTextureConstants));
    LightingBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FLighting));
    FlagBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FLitUnlitConstants));
    ViewportParamsConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FViewportParamsConstant));  
    CameraNearFarConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FCameraNearFarConstant));
    FogConstantBuffer = RenderResourceManager.CreateConstantBuffer(sizeof(FFogConstants));
}

void FRenderer::ReleaseConstantBuffer()
{
    RenderResourceManager.ReleaseBuffer(ConstantBuffer);
    RenderResourceManager.ReleaseBuffer(SubUVConstantBuffer);
    RenderResourceManager.ReleaseBuffer(GridConstantBuffer);
    RenderResourceManager.ReleaseBuffer(LinePrimitiveBuffer);
    RenderResourceManager.ReleaseBuffer(MaterialConstantBuffer);
    RenderResourceManager.ReleaseBuffer(SubMeshConstantBuffer);
    RenderResourceManager.ReleaseBuffer(TextureConstantBufer);
    RenderResourceManager.ReleaseBuffer(LightingBuffer);
    RenderResourceManager.ReleaseBuffer(FlagBuffer);
    RenderResourceManager.ReleaseBuffer(FogConstantBuffer);
}
#pragma endregion ConstantBuffer

#pragma region

void FRenderer::ClearRenderArr()
{
    StaticMeshObjs.Empty();
    GizmoObjs.Empty();
    BillboardObjs.Empty();
    LightObjs.Empty();
}

void FRenderer::PrepareRender()
{
    if (GEngine->GetWorld()->WorldType == EWorldType::Editor)
    {
        for (const auto iter : TObjectRange<USceneComponent>())
        {
                UE_LOG(LogLevel::Display, "%d", GUObjectArray.GetObjectItemArrayUnsafe().Num());
                if (UStaticMeshComponent* pStaticMeshComp = Cast<UStaticMeshComponent>(iter))
                {
                    if (!Cast<UGizmoBaseComponent>(iter))
                        StaticMeshObjs.Add(pStaticMeshComp);
                }
                if (UGizmoBaseComponent* pGizmoComp = Cast<UGizmoBaseComponent>(iter))
                {
                    GizmoObjs.Add(pGizmoComp);
                }
                if (UBillboardComponent* pBillboardComp = Cast<UBillboardComponent>(iter))
                {
                    BillboardObjs.Add(pBillboardComp);
                }
                if (ULightComponentBase* pLightComp = Cast<ULightComponentBase>(iter))
                {
                    LightObjs.Add(pLightComp);
                }
        }
    }
    else if (GEngine->GetWorld()->WorldType == EWorldType::PIE)
    {
        // UE_LOG(LogLevel::Display, "%d", GEngine->GetWorld()->GetActors().Num() );
        for (const auto iter : GEngine->GetWorld()->GetActors())
        {
            
            for (const auto iter2 : iter->GetComponents())
            {
                if (UStaticMeshComponent* pStaticMeshComp = Cast<UStaticMeshComponent>(iter2))
                {
                    if (!Cast<UGizmoBaseComponent>(iter2))
                        StaticMeshObjs.Add(pStaticMeshComp);
                }
                if (UBillboardComponent* pBillboardComp = Cast<UBillboardComponent>(iter2))
                {
                    BillboardObjs.Add(pBillboardComp);
                }
                if (ULightComponentBase* pLightComp = Cast<ULightComponentBase>(iter2))
                {
                    LightObjs.Add(pLightComp);
                }
            }
        }
    }
}

void FRenderer::Render(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    Graphics->DeviceContext->RSSetViewports(1, &ActiveViewport->GetD3DViewport());
    Graphics->ChangeRasterizer(ActiveViewport->GetViewMode());
    ChangeViewMode(ActiveViewport->GetViewMode());
    ConstantBufferUpdater.UpdateLightConstant(LightingBuffer);

    Graphics->DeviceContext->OMSetRenderTargets(RTV_NUM, Graphics->RTVs, Graphics->DepthStencilView);

    UPrimitiveBatch::GetInstance().RenderBatch(ConstantBuffer, ActiveViewport->GetViewMatrix(), ActiveViewport->GetProjectionMatrix());

    
    

    if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Primitives))
        RenderStaticMeshes(World, ActiveViewport);

    

    if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_BillboardText))
        RenderBillboards(World, ActiveViewport);

    RenderLight(World, ActiveViewport);

    if (ActiveViewport->GetViewMode() == EViewModeIndex::VMI_Depth)
    {
        RenderDepthVisualization(ActiveViewport);
    }
    else {
        // 일단은 DepthVisualization 안 하는 경우에만 Fog
        RenderFogVisualization(ActiveViewport);
    }

    
    
    RenderGizmos(World, ActiveViewport);
    ClearRenderArr();
}

void FRenderer::RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices) const
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
    Graphics->DeviceContext->Draw(numVertices, 0);
}

void FRenderer::RenderPrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices) const
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &Stride, &offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    Graphics->DeviceContext->DrawIndexed(numIndices, 0, 0);
}

void FRenderer::RenderPrimitive(OBJ::FStaticMeshRenderData* renderData, TArray<FStaticMaterial*> materials, TArray<UMaterial*> overrideMaterial, int selectedSubMeshIndex = -1) const
{
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &renderData->VertexBuffer, &Stride, &offset);

    if (renderData->IndexBuffer)
        Graphics->DeviceContext->IASetIndexBuffer(renderData->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    if (renderData->MaterialSubsets.Num() == 0)
    {
        // no submesh
        Graphics->DeviceContext->DrawIndexed(renderData->Indices.Num(), 0, 0);
    }

    for (int subMeshIndex = 0; subMeshIndex < renderData->MaterialSubsets.Num(); subMeshIndex++)
    {
        int materialIndex = renderData->MaterialSubsets[subMeshIndex].MaterialIndex;

        subMeshIndex == selectedSubMeshIndex ? ConstantBufferUpdater.UpdateSubMeshConstant(SubMeshConstantBuffer, true) : ConstantBufferUpdater.UpdateSubMeshConstant(SubMeshConstantBuffer, false);

        overrideMaterial[materialIndex] != nullptr ?
            UpdateMaterial(overrideMaterial[materialIndex]->GetMaterialInfo()) : UpdateMaterial(materials[materialIndex]->Material->GetMaterialInfo());

        if (renderData->IndexBuffer)
        {
            // index draw
            uint64 startIndex = renderData->MaterialSubsets[subMeshIndex].IndexStart;
            uint64 indexCount = renderData->MaterialSubsets[subMeshIndex].IndexCount;
            Graphics->DeviceContext->DrawIndexed(indexCount, startIndex, 0);
        }
    }
}

void FRenderer::RenderTexturePrimitive(
    ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices, ID3D11ShaderResourceView* _TextureSRV,
    ID3D11SamplerState* _SamplerState
) const
{
    if (!_TextureSRV || !_SamplerState)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "SRV, Sampler Error");
    }
    if (numIndices <= 0)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "numIndices Error");
    }
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &TextureStride, &offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &_TextureSRV);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &_SamplerState);
    Graphics->DeviceContext->DrawIndexed(numIndices, 0, 0);
}

void FRenderer::RenderTextPrimitive(
    ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11ShaderResourceView* _TextureSRV, ID3D11SamplerState* _SamplerState
) const
{
    if (!_TextureSRV || !_SamplerState)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "SRV, Sampler Error");
    }
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &TextureStride, &offset);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &_TextureSRV);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &_SamplerState);
    Graphics->DeviceContext->Draw(numVertices, 0);
}

void FRenderer::RenderTexturedModelPrimitive(
    ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices, ID3D11ShaderResourceView* InTextureSRV,
    ID3D11SamplerState* InSamplerState
) const
{
    if (!InTextureSRV || !InSamplerState)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "SRV, Sampler Error");
    }
    if (numIndices <= 0)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "numIndices Error");
    }
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &Stride, &offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    //Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &InTextureSRV);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &InSamplerState);

    Graphics->DeviceContext->DrawIndexed(numIndices, 0, 0);
}

void FRenderer::RenderStaticMeshes(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    PrepareShader();
    for (UStaticMeshComponent* StaticMeshComp : StaticMeshObjs)
    {
        FMatrix Model = JungleMath::CreateModelMatrix(
            StaticMeshComp->GetWorldLocation(),
            StaticMeshComp->GetWorldRotation(),
            StaticMeshComp->GetWorldScale()
        );
        // 최종 MVP 행렬
        FMatrix MVP = Model * ActiveViewport->GetViewMatrix() * ActiveViewport->GetProjectionMatrix();
        // 노말 회전시 필요 행렬
        FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        FVector4 UUIDColor = StaticMeshComp->EncodeUUID() / 255.0f;
        if (World->GetSelectedActor() == StaticMeshComp->GetOwner())
        {
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, MVP, NormalMatrix, UUIDColor, true);
        }
        else
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, MVP, NormalMatrix, UUIDColor, false);

        if (USkySphereComponent* skysphere = Cast<USkySphereComponent>(StaticMeshComp))
        {
            ConstantBufferUpdater.UpdateTextureConstant(TextureConstantBufer, skysphere->UOffset, skysphere->VOffset);
        }
        else
        {
            ConstantBufferUpdater.UpdateTextureConstant(TextureConstantBufer, 0, 0);
        }

        if (ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_AABB))
        {
            UPrimitiveBatch::GetInstance().RenderAABB(
                StaticMeshComp->GetBoundingBox(),
                StaticMeshComp->GetWorldLocation(),
                Model
            );
        }


        if (!StaticMeshComp->GetStaticMesh()) continue;

        OBJ::FStaticMeshRenderData* renderData = StaticMeshComp->GetStaticMesh()->GetRenderData();
        if (renderData == nullptr) continue;

        RenderPrimitive(renderData, StaticMeshComp->GetStaticMesh()->GetMaterials(), StaticMeshComp->GetOverrideMaterials(), StaticMeshComp->GetselectedSubMeshIndex());
    }
}

void FRenderer::RenderGizmos(const UWorld* World, const std::shared_ptr<FEditorViewportClient>& ActiveViewport)
{
    if (!World->GetSelectedActor())
    {
        return;
    }

    PrepareShader();

#pragma region GizmoDepth
    ID3D11DepthStencilState* DepthStateDisable = Graphics->DepthStateDisable;
    Graphics->DeviceContext->OMSetDepthStencilState(DepthStateDisable, 0);
#pragma endregion GizmoDepth

    //  fill solid,  Wirframe 에서도 제대로 렌더링되기 위함
    Graphics->DeviceContext->RSSetState(UEditorEngine::graphicDevice.RasterizerStateSOLID);

    for (auto GizmoComp : GizmoObjs)
    {

        if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_TRANSLATION)
            continue;
        else if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_SCALE)
            continue;
        else if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_ROTATION)
            continue;
        FMatrix Model = JungleMath::CreateModelMatrix(GizmoComp->GetWorldLocation(),
            GizmoComp->GetWorldRotation(),
            GizmoComp->GetWorldScale()
        );
        FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        FVector4 UUIDColor = GizmoComp->EncodeUUID() / 255.0f;

        FMatrix MVP = Model * ActiveViewport->GetViewMatrix() * ActiveViewport->GetProjectionMatrix();

        if (GizmoComp == World->GetPickingGizmo())
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, MVP, NormalMatrix, UUIDColor, true);
        else
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, MVP, NormalMatrix, UUIDColor, false);

        if (!GizmoComp->GetStaticMesh()) continue;

        OBJ::FStaticMeshRenderData* renderData = GizmoComp->GetStaticMesh()->GetRenderData();
        if (renderData == nullptr) continue;

        RenderPrimitive(renderData, GizmoComp->GetStaticMesh()->GetMaterials(), GizmoComp->GetOverrideMaterials());
    }

    Graphics->DeviceContext->RSSetState(Graphics->GetCurrentRasterizer());

#pragma region GizmoDepth
    ID3D11DepthStencilState* originalDepthState = Graphics->DepthStencilState;
    Graphics->DeviceContext->OMSetDepthStencilState(originalDepthState, 0);
#pragma endregion GizmoDepth
}

void FRenderer::RenderBillboards(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    PrepareTextureShader();
    PrepareSubUVConstant();
    for (auto BillboardComp : BillboardObjs)
    {
        ConstantBufferUpdater.UpdateSubUVConstant(SubUVConstantBuffer, BillboardComp->finalIndexU, BillboardComp->finalIndexV);

        FMatrix Model = BillboardComp->CreateBillboardMatrix();

        // 최종 MVP 행렬
        FMatrix MVP = Model * ActiveViewport->GetViewMatrix() * ActiveViewport->GetProjectionMatrix();
        FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        FVector4 UUIDColor = BillboardComp->EncodeUUID() / 255.0f;
        if (BillboardComp == World->GetPickingGizmo())
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, MVP, NormalMatrix, UUIDColor, true);
        else
            ConstantBufferUpdater.UpdateConstant(ConstantBuffer, Model, MVP, NormalMatrix, UUIDColor, false);

        if (UParticleSubUVComp* SubUVParticle = Cast<UParticleSubUVComp>(BillboardComp))
        {
            RenderTexturePrimitive(
                SubUVParticle->vertexSubUVBuffer, SubUVParticle->numTextVertices,
                SubUVParticle->indexTextureBuffer, SubUVParticle->numIndices, SubUVParticle->Texture->TextureSRV, SubUVParticle->Texture->SamplerState
            );
        }
        else if (UText* Text = Cast<UText>(BillboardComp))
        {
            UEditorEngine::renderer.RenderTextPrimitive(
                Text->vertexTextBuffer, Text->numTextVertices,
                Text->Texture->TextureSRV, Text->Texture->SamplerState
            );
        }
        else
        {
            RenderTexturePrimitive(
                BillboardComp->vertexTextureBuffer, BillboardComp->numVertices,
                BillboardComp->indexTextureBuffer, BillboardComp->numIndices, BillboardComp->Texture->TextureSRV, BillboardComp->Texture->SamplerState
            );
        }
    }
    PrepareShader();
}

void FRenderer::CreateDepthVisualizationResources()
{
    HRESULT hr;

    // 1. 샘플러 생성
    D3D11_SAMPLER_DESC SamplerDesc = {};
    ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = 0;
    SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = Graphics->Device->CreateSamplerState(&SamplerDesc, &LinearSampler);
    if (FAILED(hr))
    {
        Console::GetInstance().AddLog(LogLevel::Error, "Failed to create linear sampler state");
        return;
    }

    // 2. 셰이더 생성(일단은 ShaderManager를 사용하지 않음)
    ID3DBlob* VSBlob = nullptr; // VS Blob을 받을 변수
    ID3DBlob* PSBlob = nullptr; // PS Blob을 받을 변수
    ID3DBlob* ErrorBlob = nullptr; // 에러 Blob을 받을 변수
    hr = D3DCompileFromFile(L"Shaders/DepthVisualizer.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "mainVS", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 0, &VSBlob, &ErrorBlob);
    if (FAILED(hr)) 
    {
        if (ErrorBlob)
        { 
            UE_LOG(LogLevel::Error, "VS Compile Error: %s", (char*)ErrorBlob->GetBufferPointer());
            ErrorBlob->Release();
        }
        else
        { 
            UE_LOG(LogLevel::Error, "Failed to compile depth VS! File not found?");
        }
        return;
    }
    hr = Graphics->Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &DepthVisualizationVS);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Error, "Failed to create depth visualization vertex shader");
        VSBlob->Release(); // VSBlob 해제
        return;
    }

    hr = D3DCompileFromFile(L"Shaders/DepthVisualizer.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "mainPS", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 0, &PSBlob, &ErrorBlob);
    if (FAILED(hr))
    {
        if (ErrorBlob)
        {
            UE_LOG(LogLevel::Error, "PS Compile Error: %s", (char*)ErrorBlob->GetBufferPointer());
            ErrorBlob->Release();
        }
        else
        {
            UE_LOG(LogLevel::Error, "Failed to compile depth PS! File not found?");
        }

        if (VSBlob)
        {
            VSBlob->Release(); // VSBlob 해제
        }
        return;
    }
    hr = Graphics->Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &DepthVisualizationPS);
    PSBlob->Release(); // PS Blob은 더 이상 필요 없음
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Error, "Failed to create depth visualization pixel shader");
        if (VSBlob)
        {
            VSBlob->Release(); // VSBlob 해제
        }

        return;
    }

    // 3. 정점 버퍼 생성
    FVertexTexture Vertices[] = {
        { -1.0f,  1.0f, 0.0f, 0.0f, 0.0f },
        { 1.0f,  1.0f, 0.0f, 1.0f, 0.0f },
        { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, -1.0f, 0.0f, 1.0f, 1.0f }
    };

    D3D11_BUFFER_DESC BufferDesc = {};
    ZeroMemory(&BufferDesc, sizeof(BufferDesc));
    BufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 정적 데이터
    BufferDesc.ByteWidth = sizeof(Vertices) * 4;
    BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags = 0; 

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = Vertices;

    hr = Graphics->Device->CreateBuffer(&BufferDesc, &InitData, &QuadVertexBuffer);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Error, "Failed to create quad vertex buffer");
        if (VSBlob)
        {
            VSBlob->Release(); // VSBlob 해제
        }
        return;
    }

    // 4. Input Layout 생성
    D3D11_INPUT_ELEMENT_DESC Layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT NumElements = ARRAYSIZE(Layout);

    if (VSBlob)
    {
        hr = Graphics->Device->CreateInputLayout(Layout, NumElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &QuadInputLayout);
        VSBlob->Release(); // InputLayout 생성 후 VSBlob 해제
        if (FAILED(hr))
        {
            UE_LOG(LogLevel::Error, "Failed to create input layout for depth visualization");
            return;
        }
    }
    else
    {
        UE_LOG(LogLevel::Error, "VSBlob is null after creating input layout");
        return;
    }

    if (ErrorBlob)
    {
        ErrorBlob->Release(); // ErrorBlob 해제
    }
}

void FRenderer::CreateFogResources()
{
    HRESULT hr;
    ID3DBlob* PSBlob = nullptr; // PS Blob을 받을 변수
    ID3DBlob* ErrorBlob = nullptr; // 에러 Blob을 받을 변수
    hr = D3DCompileFromFile(L"Shaders/FogPS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 0, &PSBlob, &ErrorBlob);
    if (FAILED(hr)) {
        if (ErrorBlob) {
            UE_LOG(LogLevel::Error, "PS Compile Error: %s", (char*)ErrorBlob->GetBufferPointer());
            ErrorBlob->Release();
            return;
        }
        else
        {
            UE_LOG(LogLevel::Error, "Failed to compile depth PS! File not found?");
            return;
        }
    }
    hr = Graphics->Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &FogPS);
    if (FAILED(hr))
    {
        UE_LOG(LogLevel::Error, "Failed to create depth visualization pixel shader");
        return;
    }

    PSBlob->Release(); // PS Blob은 더 이상 필요 없음
}

void FRenderer::ReleaseFogResources()
{
    if (FogPS) {
        FogPS->Release();
        FogPS = nullptr;
    }
}

void FRenderer::PrepareFogVisualization(std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    UINT Stride = sizeof(FVertexTexture);
    UINT Offset = 0;

    // OMSet을 통해서 ScreenColorBuffer를 RTV로 등록한 것을 제거하고, SRV로 사용해야 오류가 안남
    Graphics->DeviceContext->OMSetRenderTargets(1, &Graphics->FrameBufferRTV, nullptr);

    Graphics->DeviceContext->IASetInputLayout(QuadInputLayout);
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &QuadVertexBuffer, &Stride, &Offset);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


    Graphics->DeviceContext->VSSetShader(DepthVisualizationVS, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(FogPS, nullptr, 0);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &LinearSampler);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &Graphics->pSceneSRV);
    Graphics->DeviceContext->PSSetShaderResources(1, 1, &Graphics->pPositionSRV);
    Graphics->DeviceContext->PSSetShaderResources(2, 1, &Graphics->DepthSRV);

    
    Graphics->DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void FRenderer::PrepareFogConstant(std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    // Viewport 업데이트
    float TotalWidth = Graphics->screenWidth;
    float TotalHeight = Graphics->screenHeight;
    
    FViewportParamsConstant ViewportParamsConstant;
    ViewportParamsConstant.ViewportScale.x = ActiveViewport->GetD3DViewport().Width / TotalWidth;
    ViewportParamsConstant.ViewportScale.y = ActiveViewport->GetD3DViewport().Height / TotalHeight;

    ViewportParamsConstant.ViewportOffset.x = ActiveViewport->GetD3DViewport().TopLeftX / TotalWidth;
    ViewportParamsConstant.ViewportOffset.y = ActiveViewport->GetD3DViewport().TopLeftY / TotalHeight;

    ConstantBufferUpdater.UpdateViewportParamsConstant(ViewportParamsConstantBuffer, ViewportParamsConstant);

    Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &FogConstantBuffer);
    Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &ViewportParamsConstantBuffer);
}

void FRenderer::RenderFogVisualization(std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    UFogComponent* Fog = GEngine->GetWorld()->GetFogComponent();
    if (Fog) {
        ConstantBufferUpdater.UpdateFogConstant(FogConstantBuffer, GEngine->GetWorld()->GetFogComponent()->curFogConstant);
    }
    
    PrepareFogVisualization(ActiveViewport);
    PrepareFogConstant(ActiveViewport);
    Graphics->DeviceContext->Draw(4, 0);

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    // FIXME : 사용 slot 확인후 해제해주기
    //Graphics -> DeviceContext -> PSSetShaderResources(0, 1, )
    // TODO : 빼도되는지 확인하기
   
    // FOG 이후에는 PostProcess 단계이므로 FrameBufferRTV를 그리도록
    // 만약 FrameBufferRTV를 사용하지 않으면, ImGUI가 이상한 곳에 그리게 된다.
    // SwapChain의 BackBuffer가 아닌 곳에 그리게 되기에 ImGUI가 그려지지 않음
    Graphics->DeviceContext->OMSetRenderTargets(1, &Graphics->FrameBufferRTV, nullptr);

}

void FRenderer::ReleaseDepthVisualizationResources()
{
    if (LinearSampler)
    {
        LinearSampler->Release();
        LinearSampler = nullptr;
    }
    if (DepthVisualizationVS)
    {
        DepthVisualizationVS->Release();
        DepthVisualizationVS = nullptr;
    }
    if (DepthVisualizationPS)
    {
        DepthVisualizationPS->Release();
        DepthVisualizationPS = nullptr;
    }
    if (QuadVertexBuffer)
    {
        QuadVertexBuffer->Release();
        QuadVertexBuffer = nullptr;
    }
    if (QuadInputLayout)
    {
        QuadInputLayout->Release();
        QuadInputLayout = nullptr;
    }
}

void FRenderer::PrepareDepthVisualization(std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    // 1. Render Target 설정 (GraphicsDevice에 함수를 만들어서 호출)
    Graphics->DeviceContext->OMSetRenderTargets(1, &Graphics->FrameBufferRTV, nullptr);
    // 2. Viewport 설정
    //Graphics->DeviceContext->RSSetViewports(1, &ActiveViewport->GetD3DViewport());

    // 3. 입력 어셈블러 설정
    UINT Stride = sizeof(FVertexTexture);
    UINT Offset = 0;
    Graphics->DeviceContext->IASetInputLayout(QuadInputLayout);
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &QuadVertexBuffer, &Stride, &Offset);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 4. 셰이더 및 리소스 설정
    Graphics->DeviceContext->VSSetShader(DepthVisualizationVS, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(DepthVisualizationPS, nullptr, 0);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &LinearSampler);
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &(Graphics->DepthSRV));

    if (CameraNearFarConstantBuffer && ViewportParamsConstantBuffer)
    {
        FCameraNearFarConstant CameraNearFarConstant;
        CameraNearFarConstant.NearPlane = ActiveViewport->GetNearClip();
        CameraNearFarConstant.FarPlane = ActiveViewport->GetFarClip();
        // 카메라 Near/Far 값 업데이트
        ConstantBufferUpdater.UpdateCameraNearFarConstant(CameraNearFarConstantBuffer, CameraNearFarConstant);

        float TotalWidth = Graphics->screenWidth;
        float TotalHeight = Graphics->screenHeight;

        FViewportParamsConstant ViewportParamsConstant;
        ViewportParamsConstant.ViewportScale.x = ActiveViewport->GetD3DViewport().Width / TotalWidth;
        ViewportParamsConstant.ViewportScale.y = ActiveViewport->GetD3DViewport().Height / TotalHeight;

        ViewportParamsConstant.ViewportOffset.x = ActiveViewport->GetD3DViewport().TopLeftX / TotalWidth;
        ViewportParamsConstant.ViewportOffset.y = ActiveViewport->GetD3DViewport().TopLeftY / TotalHeight;

        // 뷰포트 파라미터 업데이트
        ConstantBufferUpdater.UpdateViewportParamsConstant(ViewportParamsConstantBuffer, ViewportParamsConstant);

        Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &CameraNearFarConstantBuffer);
        Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &ViewportParamsConstantBuffer);
    }

    // 5. 상태 설정
    Graphics->DeviceContext->OMSetDepthStencilState(Graphics->DepthStateDisable, 1);
    Graphics->DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void FRenderer::RenderDepthVisualization(std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    PrepareDepthVisualization(ActiveViewport);
    
    // 렌더링
    Graphics->DeviceContext->Draw(4, 0);

    // 사용한 SRV 
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    Graphics->DeviceContext->PSSetShaderResources(0, 1, nullSRV);

}


void FRenderer::RenderLight(UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    for (auto Light : LightObjs)
    {
        FMatrix Model = JungleMath::CreateModelMatrix(Light->GetWorldLocation(), Light->GetWorldRotation(), { 1, 1, 1 });
        UPrimitiveBatch::GetInstance().AddCone(Light->GetWorldLocation(), Light->GetRadius(), 15, 140, Light->GetColor(), Model);
        UPrimitiveBatch::GetInstance().RenderOBB(Light->GetBoundingBox(), Light->GetWorldLocation(), Model);
    }
}

void FRenderer::RenderBatch(
    const FGridParameters& gridParam, ID3D11Buffer* pVertexBuffer, int boundingBoxCount, int coneCount, int coneSegmentCount, int obbCount
) const
{
    UINT stride = sizeof(FSimpleVertex);
    UINT offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    UINT vertexCountPerInstance = 2;
    UINT instanceCount = gridParam.numGridLines + 3 + (boundingBoxCount * 12) + (coneCount * (2 * coneSegmentCount)) + (12 * obbCount);
    Graphics->DeviceContext->DrawInstanced(vertexCountPerInstance, instanceCount, 0, 0);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
#pragma endregion Render

void FRenderer::ChangeViewMode(EViewModeIndex evi) const
{
    switch (evi)
    {
    case EViewModeIndex::VMI_Lit:
        ConstantBufferUpdater.UpdateLitUnlitConstant(FlagBuffer, 1);
        break;
    case EViewModeIndex::VMI_Wireframe:
    case EViewModeIndex::VMI_Unlit:
        ConstantBufferUpdater.UpdateLitUnlitConstant(FlagBuffer, 0);
    case EViewModeIndex::VMI_Depth:
        break;
    }
}

void FRenderer::UpdateMaterial(const FObjMaterialInfo& MaterialInfo) const
{
    ConstantBufferUpdater.UpdateMaterialConstant(MaterialConstantBuffer, MaterialInfo);

    if (MaterialInfo.bHasTexture == true)
    {
        std::shared_ptr<FTexture> texture = UEditorEngine::resourceMgr.GetTexture(MaterialInfo.DiffuseTexturePath);
        Graphics->DeviceContext->PSSetShaderResources(0, 1, &texture->TextureSRV);
        Graphics->DeviceContext->PSSetSamplers(0, 1, &texture->SamplerState);
    }
    else
    {
        ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
        ID3D11SamplerState* nullSampler[1] = {nullptr};

        Graphics->DeviceContext->PSSetShaderResources(0, 1, nullSRV);
        Graphics->DeviceContext->PSSetSamplers(0, 1, nullSampler);
    }
}



ID3D11ShaderResourceView* FRenderer::CreateBoundingBoxSRV(ID3D11Buffer* pBoundingBoxBuffer, UINT numBoundingBoxes)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = numBoundingBoxes;


    Graphics->Device->CreateShaderResourceView(pBoundingBoxBuffer, &srvDesc, &pBBSRV);
    return pBBSRV;
}

ID3D11ShaderResourceView* FRenderer::CreateOBBSRV(ID3D11Buffer* pBoundingBoxBuffer, UINT numBoundingBoxes)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = numBoundingBoxes;
    Graphics->Device->CreateShaderResourceView(pBoundingBoxBuffer, &srvDesc, &pOBBSRV);
    return pOBBSRV;
}

ID3D11ShaderResourceView* FRenderer::CreateConeSRV(ID3D11Buffer* pConeBuffer, UINT numCones)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN; 
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = numCones;


    Graphics->Device->CreateShaderResourceView(pConeBuffer, &srvDesc, &pConeSRV);
    return pConeSRV;
}





void FRenderer::UpdateBoundingBoxBuffer(ID3D11Buffer* pBoundingBoxBuffer, const TArray<FBoundingBox>& BoundingBoxes, int numBoundingBoxes) const
{
    if (!pBoundingBoxBuffer) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(pBoundingBoxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = reinterpret_cast<FBoundingBox*>(mappedResource.pData);
    for (int i = 0; i < BoundingBoxes.Num(); ++i)
    {
        pData[i] = BoundingBoxes[i];
    }
    Graphics->DeviceContext->Unmap(pBoundingBoxBuffer, 0);
}

void FRenderer::UpdateOBBBuffer(ID3D11Buffer* pBoundingBoxBuffer, const TArray<FOBB>& BoundingBoxes, int numBoundingBoxes) const
{
    if (!pBoundingBoxBuffer) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(pBoundingBoxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = reinterpret_cast<FOBB*>(mappedResource.pData);
    for (int i = 0; i < BoundingBoxes.Num(); ++i)
    {
        pData[i] = BoundingBoxes[i];
    }
    Graphics->DeviceContext->Unmap(pBoundingBoxBuffer, 0);
}

void FRenderer::UpdateConesBuffer(ID3D11Buffer* pConeBuffer, const TArray<FCone>& Cones, int numCones) const
{
    if (!pConeBuffer) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Graphics->DeviceContext->Map(pConeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = reinterpret_cast<FCone*>(mappedResource.pData);
    for (int i = 0; i < Cones.Num(); ++i)
    {
        pData[i] = Cones[i];
    }
    Graphics->DeviceContext->Unmap(pConeBuffer, 0);
}

void FRenderer::UpdateGridConstantBuffer(const FGridParameters& gridParams) const
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = Graphics->DeviceContext->Map(GridConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        memcpy(mappedResource.pData, &gridParams, sizeof(FGridParameters));
        Graphics->DeviceContext->Unmap(GridConstantBuffer, 0);
    }
    else
    {
        UE_LOG(LogLevel::Warning, "gridParams");
    }
}

void FRenderer::UpdateLinePrimitveCountBuffer(int numBoundingBoxes, int numCones) const
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = Graphics->DeviceContext->Map(LinePrimitiveBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto pData = static_cast<FPrimitiveCounts*>(mappedResource.pData);
    pData->BoundingBoxCount = numBoundingBoxes;
    pData->ConeCount = numCones;
    Graphics->DeviceContext->Unmap(LinePrimitiveBuffer, 0);
}