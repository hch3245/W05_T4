#pragma once
#include "Container/Array.h"

class ULightComponent;
class FLightManager
{
public:
    FLightManager();
    ~FLightManager();
    
    void AddLight(ULightComponent* Light);
    void UpdateLightConstant();
    
private:
    TArray<ULightComponent*> Lights;

};

