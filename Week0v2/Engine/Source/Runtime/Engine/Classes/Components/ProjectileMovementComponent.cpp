#include "ProjectileMovementComponent.h"
#include "Math/Quat.h"
#include "GameFramework/Actor.h"
UProjectileMovementComponent::UProjectileMovementComponent()
{
}

UProjectileMovementComponent::UProjectileMovementComponent(const UProjectileMovementComponent& other):
    UMovementComponent(other),Direction(other.Direction),InitialSpeed(other.InitialSpeed),
    MaxLifeTime(other.MaxLifeTime),LifeTime(0)
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

UObject* UProjectileMovementComponent::Duplicate() const
{
    UProjectileMovementComponent* ClonePJMove =
        FObjectFactory::ConstructObjectFrom<UProjectileMovementComponent>(this);
    ClonePJMove->DuplicateSubObjects(this);
    ClonePJMove->PostDuplicate();
    return ClonePJMove;
}

void UProjectileMovementComponent::DuplicateSubObjects(const UObject* Source)
{
    UMovementComponent::DuplicateSubObjects(Source);
}

void UProjectileMovementComponent::PostDuplicate()
{
    UMovementComponent::PostDuplicate();
}
