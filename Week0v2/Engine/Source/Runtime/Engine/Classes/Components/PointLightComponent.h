#pragma once
#include "LightComponent.h"
class UPointLightComponent :
    public ULightComponentBase
{
    DECLARE_CLASS(UPointLightComponent, ULightComponentBase)
public:
    UPointLightComponent();
    UPointLightComponent(const UPointLightComponent& other);
    virtual ~UPointLightComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual void FillLightConstant(FLightConstants& outConstant) override;
public:
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate();
private:
    FVector Position;
    float Radius = 30.0f;
    const Light::Type type = Light::Type::Point;
public:
};

