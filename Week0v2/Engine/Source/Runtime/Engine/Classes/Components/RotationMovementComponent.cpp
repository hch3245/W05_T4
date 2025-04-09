#include "RotationMovementComponent.h"
#include "Math/Quat.h"
#include "GameFramework/Actor.h"
URotationMovementComponent::URotationMovementComponent()
{
}

URotationMovementComponent::URotationMovementComponent(const URotationMovementComponent& other):
    UMovementComponent(other), PitchSpeed(other.PitchSpeed),YawSpeed(other.YawSpeed),RollSpeed(other.RollSpeed)
{
}

URotationMovementComponent::~URotationMovementComponent()
{
}

void URotationMovementComponent::TickComponent(float DeltaTime)
{
    FQuat QuatPitch(FVector(1, 0, 0), PitchSpeed * DeltaTime/100); // X축 회전
    FQuat QuatYaw(FVector(0, 0, 1), YawSpeed * DeltaTime/100);     // Z축 회전
    FQuat QuatRoll(FVector(0, 1, 0), RollSpeed * DeltaTime/100);   // Y축 회전

    FQuat DeltaRotation = QuatRoll * QuatPitch * QuatYaw;

    AActor* owner = GetOwner();
    FVector vecRot = owner->GetActorRotation();
    FQuat CurrentRotation = FQuat::CreateRotation(vecRot.x, vecRot.y, vecRot.z);

    FQuat WorldRotation = DeltaRotation * CurrentRotation;
    owner->SetActorRotation(WorldRotation.ToEulerDegrees());
    Super::TickComponent(DeltaTime);
}

float URotationMovementComponent::GetPitchSpeed()
{
    return PitchSpeed;
}

float URotationMovementComponent::GetYawSpeed()
{
    return YawSpeed;
}

float URotationMovementComponent::GetRollSpeed()
{
    return RollSpeed;
}

void URotationMovementComponent::SetPitchSpeed(float newSpeed)
{
    PitchSpeed = newSpeed;
}

void URotationMovementComponent::SetYawSpeed(float newSpeed)
{
    YawSpeed = newSpeed;
}

void URotationMovementComponent::SetRollSpeed(float newSpeed)
{
    RollSpeed = newSpeed;
}

UObject* URotationMovementComponent::Duplicate() const
{
    URotationMovementComponent* CloneRTMove =
        FObjectFactory::ConstructObjectFrom<URotationMovementComponent>(this);
    CloneRTMove->DuplicateSubObjects(this);
    CloneRTMove->PostDuplicate();
    return CloneRTMove;
}

void URotationMovementComponent::DuplicateSubObjects(const UObject* Source)
{
    UMovementComponent::DuplicateSubObjects(Source);
}

void URotationMovementComponent::PostDuplicate()
{
    UMovementComponent::PostDuplicate();
}
