#include "LightComponent.h"
#include "UBillboardComponent.h"
#include "Math/JungleMath.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "UObject/ObjectFactory.h"
ULightComponentBase::ULightComponentBase()
{
    AABB.max = { 1.f,1.f,0.1f };
    AABB.min = { -1.f,-1.f,-0.1f };
}

ULightComponentBase::ULightComponentBase(const ULightComponentBase& Other): 
    USceneComponent(Other), color(Other.color),Intensity(Other.Intensity),radius(Other.radius), AABB(Other.AABB)
{
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

UObject* ULightComponentBase::Duplicate() const
{
    ULightComponentBase* CloneLight = FObjectFactory::ConstructObjectFrom<ULightComponentBase>(this);
    CloneLight->DuplicateSubObjects(this);
    CloneLight->PostDuplicate();
    return CloneLight;
}

void ULightComponentBase::DuplicateSubObjects(const UObject* Source)
{
    USceneComponent::DuplicateSubObjects(Source);
}

void ULightComponentBase::PostDuplicate()
{
    USceneComponent::PostDuplicate();
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

