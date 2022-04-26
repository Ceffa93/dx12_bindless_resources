#pragma once
#include "Sample.h"
#include <External/d3dx12.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace DirectX;

using Microsoft::WRL::ComPtr;

class Renderer
{
    friend class Sample;
public:
    Renderer(HINSTANCE hInstance, int nCmdShow);
    ~Renderer();

    void OnUpdate();
    void OnRender();

private:
    static const UINT FrameCount = 2;
    static const UINT WindowWidth = 720;
    static const UINT WindowHeight = 360;

    std::wstring m_assetPath;
    HWND m_hwnd;
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<IDXGIFactory4> m_factory;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    UINT m_rtvDescriptorSize;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    UINT m_frameIndex;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    Sample m_sample;

    void WaitForPreviousFrame();
};
