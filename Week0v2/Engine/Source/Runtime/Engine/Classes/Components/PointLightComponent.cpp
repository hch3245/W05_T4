#include "PointLightComponent.h"
#include "Math/JungleMath.h"
#include "UObject/ObjectFactory.h"
UPointLightComponent::UPointLightComponent()
{
}

UPointLightComponent::UPointLightComponent(const UPointLightComponent& other)
    :ULightComponentBase(other),Position(other.Position),Radius(other.Radius),type(other.type)
{
}

UPointLightComponent::~UPointLightComponent()
{
}

void UPointLightComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UPointLightComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

void UPointLightComponent::FillLightConstant(FLightConstants& outConstant)
{
    const FMatrix& M = GetWorldTransform();
    outConstant.Position = GetWorldLocation();
    outConstant.Radius = Radius;
    outConstant.Type = type;
    Super::FillLightConstant(outConstant);
}

UObject* UPointLightComponent::Duplicate() const
{
    UPointLightComponent* ClonePoint = FObjectFactory::ConstructObjectFrom<UPointLightComponent>(this);
    ClonePoint->DuplicateSubObjects(this);
    ClonePoint->PostDuplicate();
    return ClonePoint;
}

void UPointLightComponent::DuplicateSubObjects(const UObject* Source)
{
    ULightComponentBase::DuplicateSubObjects(Source);
}

void UPointLightComponent::PostDuplicate()
{
    ULightComponentBase::PostDuplicate();
}

