#include "LightComponent.h"
#include "UBillboardComponent.h"
#include "Math/JungleMath.h"
#include "UnrealEd/PrimitiveBatch.h"

ULightComponentBase::ULightComponentBase()
{
    AABB.max = { 1.f,1.f,0.1f };
    AABB.min = { -1.f,-1.f,-0.1f };
}

ULightComponentBase::~ULightComponentBase()
{
}
void ULightComponentBase::SetColor(FVector4 newColor)
{
    color = newColor;
}

FVector4 ULightComponentBase::GetColor() const
{
    return color;
}

float ULightComponentBase::GetRadius() const
{
    return radius;
}

void ULightComponentBase::SetRadius(float r)
{
    radius = r;
}

void ULightComponentBase::SetIntensity(float intensity)
{
    Intensity = intensity;
}

float ULightComponentBase::GetIntensity() const
{
    return Intensity;
}

void ULightComponentBase::InitializeLight()
{
}

void ULightComponentBase::InitializeComponent()
{
    Super::InitializeComponent();
}

void ULightComponentBase::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

int ULightComponentBase::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    bool res =AABB.Intersect(rayOrigin, rayDirection, pfNearHitDistance);
    return res;
}

void ULightComponentBase::FillLightConstant(FLightConstants& outConstant)
{
    outConstant.Color = color;
    outConstant.Intensity = Intensity;
}

