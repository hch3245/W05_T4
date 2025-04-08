#include "SpotLightComponent.h"
#include "Math/JungleMath.h"
#include "UObject/ObjectFactory.h"
USpotLightComponent::USpotLightComponent(): Direction(1,0,0)
{
}

USpotLightComponent::USpotLightComponent(const USpotLightComponent& other)
    :ULightComponentBase(other), Position(other.Position),Direction(other.Direction),
    Radius(other.Radius),Angles(other.Angles),type(other.type)
{
}

USpotLightComponent::~USpotLightComponent()
{
}

void USpotLightComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void USpotLightComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

void USpotLightComponent::FillLightConstant(FLightConstants& outConstant)
{
    FVector v = FVector(1, 1, 1);
    const FMatrix& M = GetWorldTransform();
    const FQuat& Q = GetWorldQuaternion();
    //outConstant.Position = FMatrix::TransformVector(v, M);
    outConstant.Direction = FMatrix::TransformVector(GetForwardVector(), Q.ToMatrix());
    outConstant.Position = GetWorldLocation();
    //outConstant.Direction = GetForwardVector();
    outConstant.Radius = Radius;
    outConstant.SpotAngle = Angles;
    outConstant.Type = type;
    Super::FillLightConstant(outConstant);
}

UObject* USpotLightComponent::Duplicate() const
{
    USpotLightComponent* CloneSpot = FObjectFactory::ConstructObjectFrom<USpotLightComponent>(this);
    CloneSpot->DuplicateSubObjects(this);
    CloneSpot->PostDuplicate();
    return CloneSpot;
}

void USpotLightComponent::DuplicateSubObjects(const UObject* Source)
{
    ULightComponentBase::DuplicateSubObjects(Source);
}

void USpotLightComponent::PostDuplicate()
{
    ULightComponentBase::PostDuplicate();
}



