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
            FVector DeltaMove = Direction * InitialSpeed * DeltaTime*0.0001;
            FVector currentLocation = owner->GetActorLocation();
            FVector NewLocation = currentLocation + DeltaMove;
            owner->SetActorLocation(NewLocation);
            LifeTime += DeltaTime / 1000;
        }
    }

    Super::TickComponent(DeltaTime);
}

void UProjectileMovementComponent::SetDirection(const FVector& Direction)
{
    this->Direction = Direction;
}

void UProjectileMovementComponent::SetInitialSpeed(float InitialSpeed)
{
    this->InitialSpeed = InitialSpeed;
}

void UProjectileMovementComponent::SetMaxLifeTime(float MaxLifeTime)
{
    this->MaxLifeTime = MaxLifeTime;
}

void UProjectileMovementComponent::SetLifeTime(float LifeTime)
{
    this->LifeTime = LifeTime;
}

const FVector& UProjectileMovementComponent::GetDirection() const
{
    return Direction;
}

float UProjectileMovementComponent::GetInitialSpeed() const
{
    return InitialSpeed;
}

float UProjectileMovementComponent::GetMaxLifeTime() const
{
    return MaxLifeTime;
}

float UProjectileMovementComponent::GetLifeTime() const
{
    return LifeTime;
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
