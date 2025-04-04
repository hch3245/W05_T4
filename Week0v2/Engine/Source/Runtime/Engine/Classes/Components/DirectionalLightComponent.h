#pragma once
#include "LightComponent.h"
class UDirectionalLightComponent : public ULightComponentBase
{
    DECLARE_CLASS(UDirectionalLightComponent, ULightComponentBase)
public:
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual void FillLightConstant(FLightConstants& outConstant) override;
private:
    FVector Direction;
    const Light::Type type = Light::Type::Directional;
};

