#pragma once
#include "LightComponent.h"
class UPointLightComponent :
    public ULightComponentBase
{
    DECLARE_CLASS(UPointLightComponent, ULightComponentBase)
public:
    UPointLightComponent();
    virtual ~UPointLightComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual void FillLightConstant(FLightConstants& outConstant) override;
private:
    FVector Position;
    float Radius = 30.0f;
    const Light::Type type = Light::Type::Point;
public:
};

