#include "MovementComponent.h"
#include "UObject/ObjectFactory.h"
UMovementComponent::UMovementComponent()
{
}

UMovementComponent::UMovementComponent(const UMovementComponent& other):
    UActorComponent(other)
{
}

UMovementComponent::~UMovementComponent()
{
}

void UMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UMovementComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

UObject* UMovementComponent::Duplicate() const
{
    UMovementComponent* CloneMove = FObjectFactory::ConstructObjectFrom<UMovementComponent>(this);
    CloneMove->DuplicateSubObjects(this);
    CloneMove->Duplicate();
    return CloneMove;
}

void UMovementComponent::DuplicateSubObjects(const UObject* Source)
{
    UActorComponent::DuplicateSubObjects(Source);
}

void UMovementComponent::PostDuplicate()
{
    UActorComponent::PostDuplicate();
}
