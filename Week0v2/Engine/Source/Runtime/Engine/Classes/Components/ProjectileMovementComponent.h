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

    void SetDirection(const FVector& Direction);
    void SetInitialSpeed(float InitialSpeed);
    void SetMaxLifeTime(float MaxLifeTime);
    void SetLifeTime(float LifeTime);

    const FVector& GetDirection() const;
    float GetInitialSpeed() const;
    float GetMaxLifeTime() const;
    float GetLifeTime() const;
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

