#include "ProjectileMovementComponent.h"
#include "Math/Quat.h"
#include "GameFramework/Actor.h"
UProjectileMovementComponent::UProjectileMovementComponent()
{
}

UProjectileMovementComponent::~UProjectileMovementComponent()
{
}

void UProjectileMovementComponent::TickComponent(float DeltaTime)
{

    AActor* owner = GetOwner();
    if (LifeTime <= MaxLifeTime)
    {
        FVector forward = owner->GetActorForwardVector();
        FVector DeltaMove = forward * InitialSpeed * DeltaTime;
        FVector currentLocation = owner->GetActorLocation();
        FVector NewLocation = currentLocation + DeltaMove;
        owner->SetActorLocation(NewLocation);
        LifeTime += DeltaTime/1000;
    }

    Super::TickComponent(DeltaTime);
}
