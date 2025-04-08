#include "StructuredBuffer.h"
#include <cassert>
#include <cstring>

FStructuredBuffer::FStructuredBuffer()
    : Buffer(nullptr), SRV(nullptr), UAV(nullptr),
    ElementSize(0), ElementCount(0), bIsDynamic(false)
{
}

FStructuredBuffer::~FStructuredBuffer()
{
    Release();
}

void FStructuredBuffer::Release()
{
    if (UAV) { UAV->Release(); UAV = nullptr; }
    if (SRV) { SRV->Release(); SRV = nullptr; }
    if (Buffer) { Buffer->Release(); Buffer = nullptr; }
}

bool FStructuredBuffer::Create(ID3D11Device* InDevice, UINT InElementSize, UINT InElementCount, bool bCreateUAV, bool bDynamic, const void* InitialData)
{
    Release();

    ElementSize = InElementSize;
    ElementCount = InElementCount;
    bIsDynamic = bDynamic;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = ElementSize * ElementCount;
    desc.Usage = bIsDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (bCreateUAV ? D3D11_BIND_UNORDERED_ACCESS : 0);
    desc.CPUAccessFlags = bIsDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = ElementSize;

    D3D11_SUBRESOURCE_DATA initData = {};
    D3D11_SUBRESOURCE_DATA* pInitData = nullptr;
    if (InitialData)
    {
        initData.pSysMem = InitialData;
        pInitData = &initData;
    }

    HRESULT hr = InDevice->CreateBuffer(&desc, pInitData, &Buffer);
    if (FAILED(hr)) return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.NumElements = ElementCount;

    hr = InDevice->CreateShaderResourceView(Buffer, &srvDesc, &SRV);
    if (FAILED(hr)) return false;

    if (bCreateUAV)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements = ElementCount;

        hr = InDevice->CreateUnorderedAccessView(Buffer, &uavDesc, &UAV);
        if (FAILED(hr)) return false;
    }

    return true;
}

void FStructuredBuffer::Update(ID3D11DeviceContext* Context, const void* Data, size_t DataSize)
{
    assert(bIsDynamic && "Update is only allowed for dynamic buffers");
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (SUCCEEDED(Context->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        memcpy(mapped.pData, Data, DataSize);
        Context->Unmap(Buffer, 0);
    }
}

void FStructuredBuffer::BindSRV(ID3D11DeviceContext* Context, UINT Slot, UINT ShaderStageFlags)
{
    if (ShaderStageFlags & (1 << 0)) // VS
        Context->VSSetShaderResources(Slot, 1, &SRV);
    if (ShaderStageFlags & (1 << 1)) // PS
        Context->PSSetShaderResources(Slot, 1, &SRV);
    if (ShaderStageFlags & (1 << 2)) // CS
        Context->CSSetShaderResources(Slot, 1, &SRV);
}

void FStructuredBuffer::BindUAV(ID3D11DeviceContext* Context, UINT Slot)
{
    Context->CSSetUnorderedAccessViews(Slot, 1, &UAV, nullptr);
}
