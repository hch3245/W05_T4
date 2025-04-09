#pragma once
#include "LightComponent.h"
class USpotLightComponent : public ULightComponentBase
{
    DECLARE_CLASS(USpotLightComponent, ULightComponentBase)
public:
    USpotLightComponent();
    USpotLightComponent(const USpotLightComponent& other);
    virtual ~USpotLightComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual void FillLightConstant(FLightConstants& outConstant) override;
public:
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate();
private:
    FVector Position;
    FVector Direction;
    float Radius = 30.0f;
    float Angles = 30.0f;

    const Light::Type type = Light::Type::Spot;
};

