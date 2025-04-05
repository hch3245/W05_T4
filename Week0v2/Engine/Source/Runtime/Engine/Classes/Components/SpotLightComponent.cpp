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
    const FMatrix& Model = JungleMath::CreateModelMatrix(
        GetWorldLocation(),
        GetWorldRotation(),
        GetWorldScale()
    );
    outConstant.Position = FMatrix::TransformVector(Position, Model);
    outConstant.Direction = FMatrix::TransformVector(Direction, Model);
    outConstant.Radius = Radius;
    outConstant.SpotAngle = Angles;
    outConstant.Type = type;
    Super::FillLightConstant(outConstant);
}



