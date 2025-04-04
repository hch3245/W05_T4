#include "FLightManager.h"

FLightManager::FLightManager()
{
}

FLightManager::~FLightManager()
{
    for (int i = 0; i < Lights.Num(); i++)
    {
        delete Lights[i];
    }
    Lights.Empty();
}

void FLightManager::AddLight(ULightComponent* Light)
{
    
}

void FLightManager::UpdateLightConstant()
{
}
