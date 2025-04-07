#include "DirectionalLightComponent.h"
#include "Math/JungleMath.h"
UDirectionalLightComponent::UDirectionalLightComponent() : Direction(-1,-1,-1)
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
