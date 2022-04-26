#include "Renderer.h"
#include <External/DXSampleHelper.h>

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
        Renderer* pSample = reinterpret_cast<Renderer*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

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

    HWND CreateWindowHandle(HINSTANCE hInstance, UINT width, UINT height, Renderer* pSample)
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

    ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator)
    {
        ComPtr<ID3D12GraphicsCommandList> commandList;
        ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));
        return commandList;
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


Renderer::Renderer(HINSTANCE hInstance, int nCmdShow)
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
    , m_frameIndex(m_swapChain->GetCurrentBackBufferIndex())
    , m_commandList(CreateCommandList(m_device.Get(), m_commandAllocator.Get()))
    , m_fenceEvent(CreateFenceEvent())
    , m_fence(CreateFence(m_device.Get()))
    , m_fenceValue(1)
    , m_sample(*this)
{
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

Renderer::~Renderer()
{
    WaitForPreviousFrame();
    CloseHandle(m_fenceEvent);
}

void Renderer::OnUpdate()
{
    m_sample.OnUpdate();
}

void Renderer::OnRender()
{
    ThrowIfFailed(m_commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    m_sample.OnRender(m_commandList.Get());

    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    ThrowIfFailed(m_commandList->Close());

    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    ThrowIfFailed(m_swapChain->Present(1, 0));

    WaitForPreviousFrame();
}

void Renderer::WaitForPreviousFrame()
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
