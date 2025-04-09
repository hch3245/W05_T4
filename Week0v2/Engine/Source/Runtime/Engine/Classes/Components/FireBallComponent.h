#pragma once
#include "PointLightComponent.h"
#include <Define.h>
class UFireBallComponent :
    public UPointLightComponent
{
    DECLARE_CLASS(UFireBallComponent, UPointLightComponent)
public:
    UFireBallComponent();
    UFireBallComponent(const UFireBallComponent& other);
    virtual ~UFireBallComponent();
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
public:
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate();
};

