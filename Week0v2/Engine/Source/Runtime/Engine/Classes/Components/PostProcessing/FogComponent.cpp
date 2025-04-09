#include "FogComponent.h"
#include "Runtime/Engine/Classes/Components/QuadTexture.h"
#include "Runtime/Launch/EditorEngine.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/EditorViewportClient.h"

UFogComponent::UFogComponent()
{
    fullscreenQuadVB = nullptr;
    fullscreenQuadIB = nullptr;
    numVertices = 0;
    numIndices = 0;

    //FIXME : 임시 초기화 값.
    curFogConstant = new FFogConstants();

    curFogConstant->FogDensity = 0.05f;
    curFogConstant->FogHeightFalloff = 0.05f;
    curFogConstant->StartDistance = 0.0f;
    curFogConstant->FogCutOffDistance = 50.0f;
    curFogConstant->FogMaxOpacity = 1.0f;
    curFogConstant->FogInScatteringColor = FVector4(0.9f, 0.4f, 0.1f, 1.0f);
        //FVector4(0.7f, 0.8f, 1.0f, 1.0f);
}

UFogComponent::~UFogComponent()
{
    if (fullscreenQuadVB)
    {
        fullscreenQuadVB->Release();
        fullscreenQuadVB = nullptr;
    }
    if (fullscreenQuadIB)
    {
        fullscreenQuadIB->Release();
        fullscreenQuadIB = nullptr;
    }
    if (curFogConstant) 
    {
        delete curFogConstant;
        curFogConstant = nullptr;
    }

}

void UFogComponent::CreateScreenQuadVertexBuffer()
{
    numVertices = sizeof(quadTextureVertices) / sizeof(FVertexTexture);
    numIndices = sizeof(quadTextureInices) / sizeof(uint32);
    fullscreenQuadVB = UEditorEngine::renderer.GetResourceManager().CreateVertexBuffer(quadTextureVertices, sizeof(quadTextureVertices));
    fullscreenQuadIB = UEditorEngine::renderer.GetResourceManager().CreateIndexBuffer(quadTextureInices, sizeof(quadTextureInices));

    if (!fullscreenQuadVB) 
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
    }
    if (!fullscreenQuadIB) 
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
    }
}

void UFogComponent::TickComponent(float DeltaTime)
{
    
}
