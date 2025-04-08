#include "RotationMovementComponent.h"
#include "Math/Quat.h"
#include "GameFramework/Actor.h"
URotationMovementComponent::URotationMovementComponent()
{
}

URotationMovementComponent::~URotationMovementComponent()
{
}

void URotationMovementComponent::TickComponent(float DeltaTime)
{
    FQuat QuatPitch(FVector(1, 0, 0), PitchSpeed * DeltaTime); // X축 회전
    FQuat QuatYaw(FVector(0, 0, 1), YawSpeed * DeltaTime);     // Z축 회전
    FQuat QuatRoll(FVector(0, 1, 0), RollSpeed * DeltaTime);   // Y축 회전

    FQuat DeltaRotation = QuatRoll * QuatPitch * QuatYaw;

    AActor* owner = GetOwner();
    FVector vecRot = owner->GetActorRotation();
    FQuat CurrentRotation = FQuat::CreateRotation(vecRot.x, vecRot.y, vecRot.z);

    FQuat WorldRotation = DeltaRotation * CurrentRotation;
    owner->SetActorRotation(WorldRotation.ToEulerDegrees());
    Super::TickComponent(DeltaTime);
}
