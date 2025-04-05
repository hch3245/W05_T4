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
    const FMatrix& Model = JungleMath::CreateModelMatrix(
        GetWorldLocation(),
        GetWorldRotation(),
        GetWorldScale()
    );
    outConstant.Direction = FMatrix::TransformVector(Direction,Model);
    outConstant.Type = type;
    Super::FillLightConstant(outConstant);
}
