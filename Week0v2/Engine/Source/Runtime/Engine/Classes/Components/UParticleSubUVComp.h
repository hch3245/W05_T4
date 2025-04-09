#pragma once
#include "UBillboardComponent.h"

class UParticleSubUVComp : public UBillboardComponent
{
    DECLARE_CLASS(UParticleSubUVComp, UBillboardComponent)

public:
    UParticleSubUVComp();
    virtual ~UParticleSubUVComp() override;
    UParticleSubUVComp(const UParticleSubUVComp& other);

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;

    void SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn);

    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;

    ID3D11Buffer* vertexSubUVBuffer;
    UINT numTextVertices;

protected:
    bool bIsLoop = true;

private:
    int indexU = 0;
    int indexV = 0;
    float second = 0;

    int CellsPerRow;
    int CellsPerColumn;

    void UpdateVertexBuffer(const TArray<FVertexTexture>& vertices);
    void CreateSubUVVertexBuffer();
};
