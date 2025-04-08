#pragma once
#include "Define.h"
class FStructuredBuffer
{
public:
    FStructuredBuffer();
    ~FStructuredBuffer();

    bool Create(
        ID3D11Device* InDevice,
        UINT InElementSize,
        UINT InElementCount,
        bool bCreateUAV = false,
        bool bDynamic = false,
        const void* InitialData = nullptr);

    void Update(ID3D11DeviceContext* Context, const void* Data, size_t DataSize);
    void BindSRV(ID3D11DeviceContext* Context, UINT Slot, UINT ShaderStageFlags);
    void BindUAV(ID3D11DeviceContext* Context, UINT Slot);

    void Release();

    ID3D11Buffer* GetBuffer() const { return Buffer; }
    ID3D11ShaderResourceView* GetSRV() const { return SRV; }
    ID3D11UnorderedAccessView* GetUAV() const { return UAV; }

private:
    ID3D11Buffer* Buffer;
    ID3D11ShaderResourceView* SRV;
    ID3D11UnorderedAccessView* UAV;

    UINT ElementSize;
    UINT ElementCount;
    bool bIsDynamic;
};
