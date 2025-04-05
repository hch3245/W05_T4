#include "PointLightComponent.h"
#include "Math/JungleMath.h"
UPointLightComponent::UPointLightComponent()
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
    const FMatrix& Model = JungleMath::CreateModelMatrix(
        GetWorldLocation(),
        GetWorldRotation(),
        GetWorldScale()
    );
    outConstant.Position = FMatrix::TransformVector(Position,Model);
    outConstant.Radius = Radius;
    outConstant.Type = type;
    Super::FillLightConstant(outConstant);
}

