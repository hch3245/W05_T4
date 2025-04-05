#pragma once
#include "LightComponent.h"
class USpotLightComponent : public ULightComponentBase
{
    DECLARE_CLASS(USpotLightComponent, ULightComponentBase)
public:
    USpotLightComponent();
    virtual ~USpotLightComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual void FillLightConstant(FLightConstants& outConstant) override;
private:
    FVector Position;
    float Radius = 30.0f;
    float Angles = 30.0f;

    const Light::Type type = Light::Type::Spot;
};

