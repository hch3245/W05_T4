#pragma once
#include "ActorComponent.h"
class UMovementComponent :
    public UActorComponent
{
    DECLARE_CLASS(UMovementComponent, UActorComponent)
public:
    UMovementComponent();
    UMovementComponent(const UMovementComponent& other);
    virtual ~UMovementComponent();
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
public:
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate();
};

