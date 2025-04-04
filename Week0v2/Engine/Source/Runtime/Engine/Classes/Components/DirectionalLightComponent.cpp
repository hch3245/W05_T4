#include "DirectionalLightComponent.h"

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
    outConstant.Direction = Direction;
    outConstant.Type = type;
    Super::FillLightConstant(outConstant);
}
