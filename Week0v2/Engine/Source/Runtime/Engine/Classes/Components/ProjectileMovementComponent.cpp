#include "ProjectileMovementComponent.h"
#include "Math/Quat.h"
#include "GameFramework/Actor.h"
UProjectileMovementComponent::UProjectileMovementComponent()
{
}

UProjectileMovementComponent::~UProjectileMovementComponent()
{
}

void UProjectileMovementComponent::InitializeComponent()
{
    AActor* owner = GetOwner();
    if (owner)
    {
        Direction = owner->GetActorForwardVector();
    }
    Super::InitializeComponent();
}

void UProjectileMovementComponent::TickComponent(float DeltaTime)
{

    AActor* owner = GetOwner();
    if (owner)
    {
        if (LifeTime <= MaxLifeTime)
        {
            FVector DeltaMove = Direction * InitialSpeed * DeltaTime;
            FVector currentLocation = owner->GetActorLocation();
            FVector NewLocation = currentLocation + DeltaMove;
            owner->SetActorLocation(NewLocation);
            LifeTime += DeltaTime / 1000;
        }
    }

    Super::TickComponent(DeltaTime);
}
