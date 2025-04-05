#pragma once
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "Runtime/Launch/Define.h"
class UFogComponent : public UActorComponent 
{
    DECLARE_CLASS(UFogComponent, UActorComponent)

public:
    UFogComponent();
    virtual ~UFogComponent() override;

public:
    ID3D11Buffer* fullscreenQuadVB = nullptr;
    ID3D11Buffer* fullscreenQuadIB = nullptr;
    UINT numVertices;
    UINT numIndices;

    void CreateScreenQuadVertexBuffer();

    float fogStart = 0.0f;
    float fogEnd = 100.0f;
    FVector4 fogColor;

    FFogConstants* curFogConstant;
};