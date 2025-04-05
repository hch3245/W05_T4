#include "SpotLightComponent.h"
#include "Math/JungleMath.h"
USpotLightComponent::USpotLightComponent()
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
    const FMatrix& M = GetWorldTransform();
    const FQuat& Q = GetWorldQuaternion();
    outConstant.Position = FMatrix::TransformVector(Position, M);
    outConstant.Direction = FMatrix::TransformVector(Direction, M);
    outConstant.Radius = Radius;
    outConstant.SpotAngle = Angles;
    outConstant.Type = type;
    Super::FillLightConstant(outConstant);
}



