#pragma once
#include "MovementComponent.h"
class URotationMovementComponent :
    public UMovementComponent
{
    DECLARE_CLASS(URotationMovementComponent, UMovementComponent)
public:
    URotationMovementComponent();
    virtual ~URotationMovementComponent();
    virtual void TickComponent(float DeltaTime) override;
private:
    float PitchSpeed = 0.450f * (3.1415926f / 180.0f); // X축
    float YawSpeed = 0.300f * (3.1415926f / 180.0f); // Z축
    float RollSpeed = 0.600f * (3.1415926f / 180.0f); // Y축
};

