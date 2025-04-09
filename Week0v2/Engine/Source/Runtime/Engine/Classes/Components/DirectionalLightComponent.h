#pragma once
#include "LightComponent.h"
class UDirectionalLightComponent : public ULightComponentBase
{
    DECLARE_CLASS(UDirectionalLightComponent, ULightComponentBase)
public:
    UDirectionalLightComponent();
    UDirectionalLightComponent(const UDirectionalLightComponent& other);
    virtual ~UDirectionalLightComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual void FillLightConstant(FLightConstants& outConstant) override;
public:
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate();
private:
    FVector Direction;
    const Light::Type type = Light::Type::Directional;
};

