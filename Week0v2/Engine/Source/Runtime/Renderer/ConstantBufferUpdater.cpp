#include "ConstantBufferUpdater.h"
#include <Engine/Texture.h>
#include <EditorEngine.h>

void FConstantBufferUpdater::Initialize(ID3D11DeviceContext* InDeviceContext)
{
    DeviceContext = InDeviceContext;
}

void FConstantBufferUpdater::UpdateConstant(ID3D11Buffer* ConstantBuffer, const FMatrix& M, const FMatrix& MVP, const FMatrix& NormalMatrix, FVector4 UUIDColor, bool IsSelected) const
{
    if (ConstantBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;

        DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR); // update constant buffer every frame
        {
            FConstants* constants = static_cast<FConstants*>(ConstantBufferMSR.pData);
            constants->MVP = MVP;
            constants->M = M;
            constants->ModelMatrixInverseTranspose = NormalMatrix;
            constants->UUIDColor = UUIDColor;
            constants->IsSelected = IsSelected;
        }
        DeviceContext->Unmap(ConstantBuffer, 0);
    }
}

void FConstantBufferUpdater::UpdateMaterialConstant(ID3D11Buffer* MaterialConstantBuffer, const FObjMaterialInfo& MaterialInfo) const
{
    if (MaterialConstantBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;

        DeviceContext->Map(MaterialConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR); // update constant buffer every frame
        {
            FMaterialConstants* constants = static_cast<FMaterialConstants*>(ConstantBufferMSR.pData);
            constants->DiffuseColor = MaterialInfo.Diffuse;
            constants->TransparencyScalar = MaterialInfo.TransparencyScalar;
            constants->AmbientColor = MaterialInfo.Ambient;
            constants->DensityScalar = MaterialInfo.DensityScalar;
            constants->SpecularColor = MaterialInfo.Specular;
            constants->SpecularScalar = MaterialInfo.SpecularScalar;
            constants->EmmisiveColor = MaterialInfo.Emissive;
        }
        DeviceContext->Unmap(MaterialConstantBuffer, 0);
    }
}

void FConstantBufferUpdater::UpdateLightConstant(ID3D11Buffer* LightingBuffer) const
{
    if (!LightingBuffer) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    DeviceContext->Map(LightingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    {
        FLighting* constants = static_cast<FLighting*>(mappedResource.pData);
        constants->lightDirX = 1.0f;
        constants->lightDirY = 1.0f;
        constants->lightDirZ = 1.0f;
        constants->lightColorX = 1.0f;
        constants->lightColorY = 1.0f;
        constants->lightColorZ = 1.0f;
        constants->AmbientFactor = 0.06f;
    }
    DeviceContext->Unmap(LightingBuffer, 0);
}


void FConstantBufferUpdater::UpdateLitUnlitConstant(ID3D11Buffer* FlagBuffer, int isLit) const
{
    if (FlagBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;
        DeviceContext->Map(FlagBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
        auto constants = static_cast<FLitUnlitConstants*>(constantbufferMSR.pData);
        {
            constants->isLit = isLit;
        }
        DeviceContext->Unmap(FlagBuffer, 0);
    }
}

void FConstantBufferUpdater::UpdateSubMeshConstant(ID3D11Buffer* SubMeshConstantBuffer, bool isSelected) const
{
    if (SubMeshConstantBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;
        DeviceContext->Map(SubMeshConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
        FSubMeshConstants* constants = (FSubMeshConstants*)constantbufferMSR.pData;
        {
            constants->isSelectedSubMesh = isSelected;
        }
        DeviceContext->Unmap(SubMeshConstantBuffer, 0);
    }
}

void FConstantBufferUpdater::UpdateTextureConstant(ID3D11Buffer* TextureConstantBufer, float UOffset, float VOffset)
{
    if (TextureConstantBufer)
    {
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;
        DeviceContext->Map(TextureConstantBufer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
        FTextureConstants* constants = (FTextureConstants*)constantbufferMSR.pData;
        {
            constants->UOffset = UOffset;
            constants->VOffset = VOffset;
        }
        DeviceContext->Unmap(TextureConstantBufer, 0);
    }
}

void FConstantBufferUpdater::UpdateSubUVConstant(ID3D11Buffer* SubUVConstantBuffer, float _indexU, float _indexV) const
{
    if (SubUVConstantBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

        DeviceContext->Map(SubUVConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
        auto constants = static_cast<FSubUVConstant*>(constantbufferMSR.pData);
        {
            constants->indexU = _indexU;
            constants->indexV = _indexV;
        }
        DeviceContext->Unmap(SubUVConstantBuffer, 0);
    }
}

void FConstantBufferUpdater::UpdateFogConstant(ID3D11Buffer* buffer, FFogConstants* fogConstant) const
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    
    DeviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    auto constants = static_cast<FFogConstants*>(mappedResource.pData);
    {
        constants->FogDensity = fogConstant->FogDensity;
        constants->FogHeightFalloff = fogConstant->FogHeightFalloff;
        constants->StartDistance = fogConstant->StartDistance;
        constants->FogCutOffDistance = fogConstant->FogCutOffDistance;
        constants->FogMaxOpacity = fogConstant->FogMaxOpacity;
        constants->FogInScatteringColor = fogConstant->FogInScatteringColor;
        constants->CameraWorldPos = fogConstant->CameraWorldPos;
        constants->FogBaseHeight = fogConstant->FogBaseHeight;
    }
    DeviceContext->Unmap(buffer, 0);

    //memcpy(mappedResource.pData, fogConstant, sizeof(FFogConstants));
    //DeviceContext->Unmap(buffer, 0);
}
