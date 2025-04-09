#include "DirectionalLightComponent.h"
#include "Math/JungleMath.h"
#include "UObject/ObjectFactory.h"
UDirectionalLightComponent::UDirectionalLightComponent() : Direction(-1,-1,-1)
{
}

UDirectionalLightComponent::UDirectionalLightComponent(const UDirectionalLightComponent& other)
    :ULightComponentBase(other),Direction(other.Direction),type(other.type)
{
}

UDirectionalLightComponent::~UDirectionalLightComponent()
{
}

void UDirectionalLightComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UDirectionalLightComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

void UDirectionalLightComponent::FillLightConstant(FLightConstants& outConstant)
{
    const FMatrix& M = GetWorldTransform();
    //outConstant.Direction = FMatrix::TransformVector(Direction,M);
    outConstant.Direction = Direction;
    outConstant.Type = type;
    Super::FillLightConstant(outConstant);
}

UObject* UDirectionalLightComponent::Duplicate() const
{
    UDirectionalLightComponent* CloneDirectional =
        FObjectFactory::ConstructObjectFrom<UDirectionalLightComponent>(this);
    CloneDirectional->DuplicateSubObjects(this);
    CloneDirectional->PostDuplicate();
    return CloneDirectional;
}

void UDirectionalLightComponent::DuplicateSubObjects(const UObject* Source)
{
    ULightComponentBase::DuplicateSubObjects(Source);
}

void UDirectionalLightComponent::PostDuplicate()
{
    ULightComponentBase::PostDuplicate();
}
