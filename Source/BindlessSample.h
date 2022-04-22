#pragma once
#include "DescriptorManager.h"
#include <External/DXSample.h>
#include <External/d3dx12.h>
#include <array>
#include <DirectXMath.h>

using namespace DirectX;

using Microsoft::WRL::ComPtr;

class BindlessSample : public DXSample
{
public:
    BindlessSample(HINSTANCE hInstance, UINT width, UINT height, std::wstring name, int nCmdShow);
    ~BindlessSample();

    virtual void OnUpdate();
    virtual void OnRender();

private:
    static const UINT FrameCount = 2;
    static const UINT TextureWidth = 8;
    static const UINT TextureHeight = 8;
    static const UINT TextureDepth = 2;

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


    // Pipeline objects.
    ComPtr<ID3D12PipelineState> m_graphicPipelineState;
    ComPtr<ID3D12PipelineState> m_computePipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    // App resources.
    std::array<ComPtr<ID3D12Resource>,2> m_2DTextures;
    std::array<ComPtr<ID3D12Resource>,2> m_3DTextures;

    // Synchronization objects.
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    void LoadAssets();
    void PopulateCommandList();
    void WaitForPreviousFrame();
};
