#include "FireBallComponent.h"
#include "UObject/ObjectFactory.h"
UFireBallComponent::UFireBallComponent()
{
}

UFireBallComponent::UFireBallComponent(const UFireBallComponent& other)
{
}

UFireBallComponent::~UFireBallComponent()
{
}

void UFireBallComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UFireBallComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

UObject* UFireBallComponent::Duplicate() const
{
    UFireBallComponent* CloneFireBall = FObjectFactory::ConstructObjectFrom<UFireBallComponent>(this);
    CloneFireBall->DuplicateSubObjects(this);
    CloneFireBall->PostDuplicate();
    return nullptr;
}

void UFireBallComponent::DuplicateSubObjects(const UObject* Source)
{
    UPointLightComponent::DuplicateSubObjects(Source);
}

void UFireBallComponent::PostDuplicate()
{
    UPointLightComponent::PostDuplicate();
}
