#pragma once
#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Runtime/Launch/Define.h"

class UFogComponent : public USceneComponent 
{
    DECLARE_CLASS(UFogComponent, USceneComponent)

public:
    UFogComponent();
    virtual ~UFogComponent() override;

public:
    ID3D11Buffer* fullscreenQuadVB = nullptr;
    ID3D11Buffer* fullscreenQuadIB = nullptr;
    UINT numVertices;
    UINT numIndices;

    void CreateScreenQuadVertexBuffer();
    virtual void TickComponent(float DeltaTime) override;

    FFogConstants* curFogConstant;
};