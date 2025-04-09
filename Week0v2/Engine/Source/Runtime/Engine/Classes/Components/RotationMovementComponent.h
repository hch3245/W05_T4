#pragma once
#include "MovementComponent.h"
class URotationMovementComponent :
    public UMovementComponent
{
    DECLARE_CLASS(URotationMovementComponent, UMovementComponent)
public:
    URotationMovementComponent();
    URotationMovementComponent(const URotationMovementComponent& other);
    virtual ~URotationMovementComponent();
    virtual void TickComponent(float DeltaTime) override;
public:
    float GetPitchSpeed();
    float GetYawSpeed();
    float GetRollSpeed();
    void SetPitchSpeed(float newSpeed);
    void SetYawSpeed(float newSpeed);
    void SetRollSpeed(float newSpeed);
private:
    float PitchSpeed = 4.5f * (3.1415926f / 180.0f); // X축
    float YawSpeed = 3.0f * (3.1415926f / 180.0f); // Z축
    float RollSpeed = 6.0f * (3.1415926f / 180.0f); // Y축
public:
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate();
};

