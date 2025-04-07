#pragma once
#include "MovementComponent.h"
class UProjectileMovementComponent :
    public UMovementComponent
{
    DECLARE_CLASS(UProjectileMovementComponent, UMovementComponent)
public:
    UProjectileMovementComponent();
    virtual ~UProjectileMovementComponent();
    virtual void TickComponent(float DeltaTime) override;
private:
    float InitialSpeed = 0.001f;   // 유닛당 초당 거리
    float MaxLifeTime = 100.0f;       // 초
    float LifeTime = 0.0f;
};

