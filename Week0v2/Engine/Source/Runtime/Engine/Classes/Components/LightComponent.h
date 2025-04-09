#pragma once
#include "PrimitiveComponent.h"
#include "Define.h"
class UBillboardComponent;

class ULightComponentBase : public USceneComponent
{
    DECLARE_CLASS(ULightComponentBase, USceneComponent)

public:
    ULightComponentBase();
    ULightComponentBase(const ULightComponentBase& Other);
    virtual ~ULightComponentBase() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);
    virtual void FillLightConstant(FLightConstants& outConstant);
    void InitializeLight();
    void SetColor(FVector4 newColor);
    FVector4 GetColor() const;
    float GetRadius() const;
    void SetRadius(float r);
    void SetIntensity(float intensity);
    float GetIntensity() const;
public:
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate();
private:
    FVector4 color;
    float Intensity=1.0f;
    float radius=5.0f;           //SpotLight
    FBoundingBox AABB;
public:
    FBoundingBox GetBoundingBox() const {return AABB;}
    float GetRadius() {return radius;}
    FVector4 GetColor() {return color;}
};
