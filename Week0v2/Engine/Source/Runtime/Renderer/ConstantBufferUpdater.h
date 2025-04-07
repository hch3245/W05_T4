#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>
#include "Define.h"

class FConstantBufferUpdater
{
public:
    void Initialize(ID3D11DeviceContext* InDeviceContext);

    void UpdateConstant(ID3D11Buffer* ConstantBuffer, const FMatrix& MVP, const FMatrix& NormalMatrix, FVector4 UUIDColor, bool IsSelected) const;
    void UpdateMaterialConstant(ID3D11Buffer* MaterialConstantBuffer, const FObjMaterialInfo& MaterialInfo) const;
    void UpdateLightConstant(ID3D11Buffer* LightingBuffer) const;
    void UpdateLitUnlitConstant(ID3D11Buffer* FlagBuffer, int isLit) const;
    void UpdateSubMeshConstant(ID3D11Buffer* SubMeshConstantBuffer, bool isSelected) const;
    void UpdateTextureConstant(ID3D11Buffer* TextureConstantBufer, float UOffset, float VOffset);
    void UpdateSubUVConstant(ID3D11Buffer* SubUVConstantBuffer, float _indexU, float _indexV) const;
    void UpdateCameraNearFarConstant(ID3D11Buffer* CameraNearFarConstantBuffer, FCameraNearFarConstant& CameraNearFarConstant) const;
    void UpdateViewportParamsConstant(ID3D11Buffer* ViewportParamsConstantBuffer, FViewportParamsConstant& ViewportParamsConstant) const;

private:
    ID3D11DeviceContext* DeviceContext = nullptr;
};

