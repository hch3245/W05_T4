#pragma once
#include "Runtime/Engine/Classes/Components/ActorComponent.h"

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

    float fogStart;
    float fogEnd;
    FVector4 fogColor;
};