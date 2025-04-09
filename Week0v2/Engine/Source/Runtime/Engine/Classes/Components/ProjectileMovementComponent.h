#pragma once
#include "MovementComponent.h"
class UProjectileMovementComponent :
    public UMovementComponent
{
    DECLARE_CLASS(UProjectileMovementComponent, UMovementComponent)
public:
    UProjectileMovementComponent();
    UProjectileMovementComponent(const UProjectileMovementComponent& other);
    virtual ~UProjectileMovementComponent();
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
private:
    FVector Direction;
    float InitialSpeed = 0.001f;   // 유닛당 초당 거리
    float MaxLifeTime = 100.0f;       // 초
    float LifeTime = 0.0f;
public:
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate();
};

