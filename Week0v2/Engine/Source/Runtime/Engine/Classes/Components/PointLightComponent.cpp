#include "PointLightComponent.h"

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
    outConstant.Position = Poistion;
    outConstant.Radius = Radius;
    outConstant.Type = type;
    Super::FillLightConstant(outConstant);
}
