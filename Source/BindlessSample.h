#pragma once
#include "DescriptorManager.h"
#include <External/d3dx12.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <array>

using namespace DirectX;

using Microsoft::WRL::ComPtr;

class BindlessSample
{
public:
    BindlessSample(HINSTANCE hInstance, int nCmdShow);
    ~BindlessSample();

    void OnUpdate();
    void OnRender();

private:
    static const UINT FrameCount = 2;
    static const UINT TextureWidth = 8;
    static const UINT TextureHeight = 8;
    static const UINT TextureDepth = 2;
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
    DescriptorManager m_descriptorManager;
    UINT m_frameIndex;
    ComPtr<ID3D12PipelineState> m_graphicPipelineState;
    ComPtr<ID3D12PipelineState> m_computePipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    ComPtr<ID3D12Resource> m_2DTexture;
    ComPtr<ID3D12Resource> m_3DTexture;

    // Synchronization objects.
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    void PopulateCommandList();
    void WaitForPreviousFrame();
};
