#pragma once
#include "PointLightComponent.h"
#include <Define.h>
class UFireBallComponent :
    public UPointLightComponent
{
    DECLARE_CLASS(UFireBallComponent, UPointLightComponent)
public:
    UFireBallComponent();
    virtual ~UFireBallComponent();
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
};

