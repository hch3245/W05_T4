#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#define _TCHAR_DEFINED

#define RTV_NUM 3
#include <d3d11.h>

#include "EngineBaseTypes.h"

#include "Core/HAL/PlatformType.h"
#include "Core/Math/Vector4.h"

class FGraphicsDevice {
public:
    ID3D11Device* Device = nullptr;
    ID3D11DeviceContext* DeviceContext = nullptr;
    IDXGISwapChain* SwapChain = nullptr;
    ID3D11Texture2D* FrameBuffer = nullptr;
    ID3D11Texture2D* ScreenColorBuffer = nullptr;   // 선행되는 렌더링에서 나오는 결과 보관
    ID3D11Texture2D* UUIDFrameBuffer = nullptr;
    ID3D11Texture2D* PositionFrameBuffer = nullptr;
    ID3D11RenderTargetView* RTVs[RTV_NUM];
    ID3D11RenderTargetView* FrameBufferRTV = nullptr;
    ID3D11RenderTargetView* ScreenColorBufferRTV = nullptr; // 선행되는 렌더링에서 나오는 결과 받기 위한 RTV
    ID3D11RenderTargetView* UUIDFrameBufferRTV = nullptr;
    ID3D11RenderTargetView* PositionRTV = nullptr;
    ID3D11RasterizerState* RasterizerStateSOLID = nullptr;
    ID3D11RasterizerState* RasterizerStateWIREFRAME = nullptr;
    DXGI_SWAP_CHAIN_DESC SwapchainDesc;

    
    UINT screenWidth = 0;
    UINT screenHeight = 0;
    // Depth-Stencil 관련 변수
    ID3D11Texture2D* DepthStencilBuffer = nullptr;  // 깊이/스텐실 텍스처
    ID3D11DepthStencilView* DepthStencilView = nullptr;  // 깊이/스텐실 뷰
    ID3D11DepthStencilState* DepthStencilState = nullptr;
    FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // 화면을 초기화(clear) 할 때 사용할 색상(RGBA)

    ID3D11DepthStencilState* DepthStateDisable = nullptr;

    // Depth를 렌더링하기 위해 필요한 리소스 뷰
    ID3D11ShaderResourceView* DepthSRV = nullptr;
    

    void Initialize(HWND hWindow);
    void CreateDeviceAndSwapChain(HWND hWindow);
    void CreateDepthStencilBuffer(HWND hWindow);
    void CreateDepthStencilState();
    void CreateRasterizerState();
    void ReleaseDeviceAndSwapChain();
    void CreateFrameBuffer();
    void ReleaseFrameBuffer();
    void ReleaseRasterizerState();
    void ReleaseDepthStencilResources();
    void Release();
    void SwapBuffer();
    void Prepare();
    void Prepare(D3D11_VIEWPORT* viewport);
    void OnResize(HWND hWindow);
    ID3D11RasterizerState* GetCurrentRasterizer() { return CurrentRasterizer; }
    void ChangeRasterizer(EViewModeIndex evi);
    void ChangeDepthStencilState(ID3D11DepthStencilState* newDetptStencil);

    uint32 GetPixelUUID(POINT pt);
    uint32 DecodeUUIDColor(FVector4 UUIDColor);
private:
    ID3D11RasterizerState* CurrentRasterizer = nullptr;
};

