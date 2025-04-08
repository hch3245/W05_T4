#pragma once
#include "ActorComponent.h"
class UMovementComponent :
    public UActorComponent
{
    DECLARE_CLASS(UMovementComponent, UActorComponent)
public:
    UMovementComponent();
    virtual ~UMovementComponent();
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
};

