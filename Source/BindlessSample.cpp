#include "BindlessSample.h"
#include <External/DXSampleHelper.h>
#include <D3Dcompiler.h>

namespace
{
    void GetHardwareAdapter(
        _In_ IDXGIFactory1* pFactory,
        _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter)
    {
        *ppAdapter = nullptr;
        ComPtr<IDXGIAdapter1> adapter;
        ComPtr<IDXGIFactory6> factory6;
        if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            for (
                UINT adapterIndex = 0;
                SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                    adapterIndex,
                    DXGI_GPU_PREFERENCE_UNSPECIFIED,
                    IID_PPV_ARGS(&adapter)));
                ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);
                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }
                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }
        if (adapter.Get() == nullptr)
        {
            for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);
                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }
                // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }
        *ppAdapter = adapter.Detach();
    }


    LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        BindlessSample* pSample = reinterpret_cast<BindlessSample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        switch (message)
        {
        case WM_CREATE:
        {
            // Save the DXSample* passed in to CreateWindow.
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        }
        return 0;

        case WM_PAINT:
            if (pSample)
            {
                pSample->OnUpdate();
                pSample->OnRender();
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        // Handle any messages the switch statement didn't.
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    HWND CreateWindowHandle(HINSTANCE hInstance, UINT width, UINT height, BindlessSample* pSample)
    {
        WNDCLASSEX windowClass = { 0 };
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = WindowProc;
        windowClass.hInstance = hInstance;
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.lpszClassName = L"DXSampleClass";
        RegisterClassEx(&windowClass);

        RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        HWND hwnd = CreateWindow(
            windowClass.lpszClassName,
            L"Bindless",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr,
            nullptr,
            hInstance,
            pSample);

        return hwnd;
    }

    std::wstring CreateAssetPath()
    {
        WCHAR assetsPath[512];
        GetAssetsPath(assetsPath, _countof(assetsPath));
        return assetsPath;
    }

    ComPtr<IDXGIFactory4> CreateFactory(HWND hwnd)
    {
        UINT factoryFlags = 0;

#if defined(_DEBUG)
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif
        ComPtr<IDXGIFactory4> factory;
        ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)));
        ThrowIfFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
        return factory;
    }

    ComPtr<ID3D12Device> CreateDevice(IDXGIFactory4* factory)
    {
        ComPtr<ID3D12Device> device;

        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory, &hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&device)
        ));
        return device;
    }

    ComPtr<ID3D12CommandQueue> CreateCommandQueue(ID3D12Device* device)
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        ComPtr<ID3D12CommandQueue> commandQueue;
        ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
        return commandQueue;
    }

    ComPtr<IDXGISwapChain3> CreateSwapChain(HWND hwnd, UINT frameCount, UINT width, UINT height, ID3D12CommandQueue* queue, IDXGIFactory4* factory)
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = frameCount;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain;
        ThrowIfFailed(factory->CreateSwapChainForHwnd(
            queue,
            hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain
        ));
        ComPtr<IDXGISwapChain3> swapChain3;
        ThrowIfFailed(swapChain.As(&swapChain3));
        return swapChain3;
    }

    ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ID3D12Device* device)
    {
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
        return commandAllocator;
    }

    ComPtr<ID3D12DescriptorHeap> CreateRtvHeap(UINT frameCount, ID3D12Device* device)
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = frameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
        return rtvHeap;
    }

    ComPtr<ID3D12Resource> CreateRtv(LONG index, ID3D12DescriptorHeap* rtvHeap, LONG rtvDescriptorSize, ID3D12Device* device, IDXGISwapChain3* swapChain)
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
        rtvHandle.Offset(index, rtvDescriptorSize);

        ComPtr<ID3D12Resource> renderTarget;
        ThrowIfFailed(swapChain->GetBuffer(index, IID_PPV_ARGS(&renderTarget)));
        device->CreateRenderTargetView(renderTarget.Get(), nullptr, rtvHandle);
        return renderTarget;
    }

    ComPtr<ID3D12PipelineState> CreateComputePipelineState(ID3D12Device* device, const std::wstring& assetPath, DescriptorManager& descriptorManager)
    {
        UINT8* shaderData;
        UINT shaderDataLength;
        ThrowIfFailed(ReadDataFromFile((assetPath + L"cs.cso").c_str(), &shaderData, &shaderDataLength));

        D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = descriptorManager.m_rootSignature.Get();
        psoDesc.CS = CD3DX12_SHADER_BYTECODE(shaderData, shaderDataLength);
        ComPtr<ID3D12PipelineState> pipelineState;
        ThrowIfFailed(device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));

        delete shaderData;
        return pipelineState;
    }

    ComPtr<ID3D12PipelineState> CreateGraphicPipelineState(ID3D12Device* device, const std::wstring& assetPath, DescriptorManager& descriptorManager)
    {
        UINT8* pVertexShaderData;
        UINT8* pPixelShaderData;
        UINT vertexShaderDataLength;
        UINT pixelShaderDataLength;

        ThrowIfFailed(ReadDataFromFile((assetPath + L"vs.cso").c_str(), &pVertexShaderData, &vertexShaderDataLength));
        ThrowIfFailed(ReadDataFromFile((assetPath + L"ps.cso").c_str(), &pPixelShaderData, &pixelShaderDataLength));

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = descriptorManager.m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(pVertexShaderData, vertexShaderDataLength);
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pPixelShaderData, pixelShaderDataLength);
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        ComPtr<ID3D12PipelineState> pipelineState;
        ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));

        delete pVertexShaderData;
        delete pPixelShaderData;
        return pipelineState;
    }

    ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator)
    {
        ComPtr<ID3D12GraphicsCommandList> commandList;
        ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));
        return commandList;
    }

    ComPtr<ID3D12Resource> Create2DTexture(UINT textureWidth, UINT textureHeight, ID3D12Device* device, DescriptorManager& descriptorManager)
    {
        D3D12_RESOURCE_DESC desc = {};
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Width = textureWidth;
        desc.Height = textureHeight;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        desc.DepthOrArraySize = 1;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        ComPtr<ID3D12Resource> texture;
        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            nullptr,
            IID_PPV_ARGS(&texture)));

        descriptorManager.allocateTexture2DUavDescriptor(texture.Get(), desc.Format);
        descriptorManager.allocateTexture2DSrvDescriptor(texture.Get(), desc.Format);
        return texture;
    }

    ComPtr<ID3D12Resource> Create3DTexture(UINT textureWidth, UINT textureHeight, UINT textureDepth, ID3D12Device* device, DescriptorManager& descriptorManager)
    {
        D3D12_RESOURCE_DESC desc = {};
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Width = textureWidth;
        desc.Height = textureHeight;
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        desc.DepthOrArraySize = textureDepth;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;

        ComPtr<ID3D12Resource> texture;
        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            nullptr,
            IID_PPV_ARGS(&texture)));

        descriptorManager.allocateTexture3DUavDescriptor(texture.Get(), desc.Format);
        descriptorManager.allocateTexture3DSrvDescriptor(texture.Get(), desc.Format);
        return texture;
    }

    HANDLE CreateFenceEvent()
    {
        HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        return fenceEvent;
    }

    ComPtr<ID3D12Fence> CreateFence(ID3D12Device* device)
    {
        ComPtr<ID3D12Fence> fence;
        ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        return fence;
    }
}


BindlessSample::BindlessSample(HINSTANCE hInstance, int nCmdShow)
    : m_assetPath(CreateAssetPath())
    , m_hwnd(CreateWindowHandle(hInstance, WindowWidth, WindowHeight, this))
    , m_viewport(0.0f, 0.0f, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight))
    , m_scissorRect(0, 0, static_cast<LONG>(WindowWidth), static_cast<LONG>(WindowHeight))
    , m_factory(CreateFactory(m_hwnd))
    , m_device(CreateDevice(m_factory.Get()))
    , m_commandQueue(CreateCommandQueue(m_device.Get()))
    , m_swapChain(CreateSwapChain(m_hwnd, FrameCount, WindowWidth, WindowHeight, m_commandQueue.Get(), m_factory.Get()))
    , m_commandAllocator(CreateCommandAllocator(m_device.Get()))
    , m_rtvDescriptorSize(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV))
    , m_rtvHeap(CreateRtvHeap(FrameCount, m_device.Get()))
    , m_renderTargets{
        CreateRtv(0, m_rtvHeap.Get(), m_rtvDescriptorSize, m_device.Get(), m_swapChain.Get()),
        CreateRtv(1, m_rtvHeap.Get(), m_rtvDescriptorSize, m_device.Get(), m_swapChain.Get())}
    , m_descriptorManager(m_device.Get())
    , m_frameIndex(m_swapChain->GetCurrentBackBufferIndex())
    , m_graphicPipelineState(CreateGraphicPipelineState(m_device.Get(), m_assetPath, m_descriptorManager))
    , m_computePipelineState(CreateComputePipelineState(m_device.Get(), m_assetPath, m_descriptorManager))
    , m_commandList(CreateCommandList(m_device.Get(), m_commandAllocator.Get()))
    , m_2DTexture(Create2DTexture(TextureWidth, TextureHeight, m_device.Get(), m_descriptorManager))
    , m_3DTexture(Create3DTexture(TextureWidth, TextureHeight, TextureDepth, m_device.Get(), m_descriptorManager))
    , m_fenceEvent(CreateFenceEvent())
    , m_fence(CreateFence(m_device.Get()))
    , m_fenceValue(1)
{
    D3D12_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 0;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    m_descriptorManager.allocateSamplerDescriptor(samplerDesc);


    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    WaitForPreviousFrame();

    ShowWindow(m_hwnd, nCmdShow);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

BindlessSample::~BindlessSample()
{
    WaitForPreviousFrame();
    CloseHandle(m_fenceEvent);
}

void BindlessSample::OnUpdate()
{
}

void BindlessSample::OnRender()
{
    PopulateCommandList();

    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    ThrowIfFailed(m_swapChain->Present(1, 0));

    WaitForPreviousFrame();
}


void BindlessSample::PopulateCommandList()
{
    ThrowIfFailed(m_commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

    m_descriptorManager.setHeaps(m_commandList.Get());
	
    {
        m_commandList->SetPipelineState(m_computePipelineState.Get());
        m_descriptorManager.setSignature(m_commandList.Get(), true);
        m_descriptorManager.setTables(m_commandList.Get(), true);
        m_commandList->Dispatch(1,1,1);
    }

    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_2DTexture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_3DTexture.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
  
    {
        m_commandList->SetPipelineState(m_graphicPipelineState.Get());
        m_descriptorManager.setSignature(m_commandList.Get(), false);
        m_descriptorManager.setTables(m_commandList.Get(), false);

        m_commandList->RSSetViewports(1, &m_viewport);
        m_commandList->RSSetScissorRects(1, &m_scissorRect);

        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
        m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_commandList->DrawInstanced(3, 1, 0, 0);

        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_2DTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_3DTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
    }
	
    ThrowIfFailed(m_commandList->Close());
}

void BindlessSample::WaitForPreviousFrame()
{
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;

    if (m_fence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}
