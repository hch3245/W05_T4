#include "FogComponent.h"
#include "Runtime/Engine/Classes/Components/QuadTexture.h"
#include "Engine\Source\Runtime\Launch\EditorEngine.h"

UFogComponent::UFogComponent()
{
    fullscreenQuadVB = nullptr;
    fullscreenQuadIB = nullptr;
    numVertices = 0;
    numIndices = 0;

    //FIXME : 임시 초기화 값.
    curFogConstant->FogDensity = 0.03f;
    curFogConstant->FogHeightFalloff = 0.1f;
    curFogConstant->StartDistance = fogStart;
    curFogConstant->FogCutOffDistance = fogEnd;
    curFogConstant->FogMaxOpacity = 0.8f;
    curFogConstant->FogInScatteringColor = fogColor;
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
