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
    const FMatrix& M = GetWorldTransform();
    outConstant.Position = GetWorldLocation();
    outConstant.Radius = Radius;
    outConstant.Type = type;
    Super::FillLightConstant(outConstant);
}

