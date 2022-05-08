#include "Device.h"
#include <External/DXSampleHelper.h>
#include <External/d3dx12.h>
#include <array>

Device::Heap::Heap(ID3D12Device* device, unsigned int descriptorNum, D3D12_DESCRIPTOR_HEAP_TYPE heapType)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = descriptorNum;
    heapDesc.Type = heapType;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));

    cpuStart = heap->GetCPUDescriptorHandleForHeapStart().ptr;
    gpuStart = heap->GetGPUDescriptorHandleForHeapStart().ptr;
    descriptorSize = device->GetDescriptorHandleIncrementSize(heapType);
    lastAllocated = 0;
}

Device::Device(ID3D12Device* device)
    : m_resourceHeap(device, 2048, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
    , m_samplerHeap(device, 16, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
{
    m_device = device;
	
	// Create the root signature.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        std::array<CD3DX12_DESCRIPTOR_RANGE1, 4> resourcesRanges{
            CD3DX12_DESCRIPTOR_RANGE1{D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UINT_MAX, 0, 1, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0},
            CD3DX12_DESCRIPTOR_RANGE1{D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UINT_MAX, 0, 2, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0},
            CD3DX12_DESCRIPTOR_RANGE1{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0, 1, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0},
            CD3DX12_DESCRIPTOR_RANGE1{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0, 2, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0},
        };

        CD3DX12_DESCRIPTOR_RANGE1 samplerRange{D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, UINT_MAX, 0, 1, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0};

        CD3DX12_ROOT_PARAMETER1 rootParameters[16];
        for (UINT i = 0; i < 14; i++)
        {
            rootParameters[i].InitAsConstantBufferView(i, 0u, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
        }
        rootParameters[14].InitAsDescriptorTable(static_cast<int>(resourcesRanges.size()), resourcesRanges.data());
        rootParameters[15].InitAsDescriptorTable(1, &samplerRange);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
        ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
    }
}

unsigned Device::allocateResourceDescriptor()
{
    return m_resourceHeap.allocate();
}
unsigned Device::allocateSamplerDescriptor()
{
    return m_samplerHeap.allocate();
}

void Device::deallocateResourceDescriptor(unsigned int)
{
    // TODO: stack allocator does not support deallocate
}
void Device::deallocateSamplerDescriptor(unsigned int)
{
    // TODO: stack allocator does not support deallocate
}

void Device::createTexture2DUavDescriptor(unsigned int handle, ID3D12Resource* texture, DXGI_FORMAT format, D3D12_TEX2D_UAV resDesc)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
    desc.Format = format;
    desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    desc.Texture2D = resDesc;

    m_device->CreateUnorderedAccessView(texture, nullptr, &desc, m_resourceHeap.getCpuHandle(handle));
}
void Device::createTexture2DSrvDescriptor(unsigned int handle, ID3D12Resource* texture, DXGI_FORMAT format, UINT mapping, D3D12_TEX2D_SRV resDesc)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = format;
    desc.Shader4ComponentMapping = mapping;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D = resDesc;

    m_device->CreateShaderResourceView(texture, &desc, m_resourceHeap.getCpuHandle(handle));
}
void Device::createTexture3DUavDescriptor(unsigned int handle, ID3D12Resource* texture, DXGI_FORMAT format, D3D12_TEX3D_UAV resDesc)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
    desc.Format = format;
    desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
    desc.Texture3D = resDesc;

    m_device->CreateUnorderedAccessView(texture, nullptr, &desc, m_resourceHeap.getCpuHandle(handle));
}
void Device::createTexture3DSrvDescriptor(unsigned int handle, ID3D12Resource* texture, DXGI_FORMAT format, UINT mapping, D3D12_TEX3D_SRV resDesc)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = format;
    desc.Shader4ComponentMapping = mapping;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
    desc.Texture3D = resDesc;

    m_device->CreateShaderResourceView(texture, &desc, m_resourceHeap.getCpuHandle(handle));
}

void Device::createSamplerDescriptor(unsigned int handle, D3D12_SAMPLER_DESC& desc)
{
    m_device->CreateSampler(&desc, m_samplerHeap.getCpuHandle(handle));
}

void Device::setHeaps(ID3D12GraphicsCommandList* commandList)
{
    ID3D12DescriptorHeap* ppHeaps[] = { m_resourceHeap.heap.Get(), m_samplerHeap.heap.Get() };
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void Device::setSignature(ID3D12GraphicsCommandList* commandList, bool bCompute)
{
    if (bCompute)
    {
        commandList->SetComputeRootSignature(m_rootSignature.Get());
    }
    else
    {
        commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    }
}

void Device::setTables(ID3D12GraphicsCommandList* commandList, bool bCompute)
{
	D3D12_GPU_DESCRIPTOR_HANDLE resourceHandle, samplerHandle;
    resourceHandle.ptr = m_resourceHeap.gpuStart;
    samplerHandle.ptr = m_samplerHeap.gpuStart;
    if (bCompute)
    {
        commandList->SetComputeRootDescriptorTable(14, resourceHandle);
        commandList->SetComputeRootDescriptorTable(15, samplerHandle);
    }
    else
    {
        commandList->SetGraphicsRootDescriptorTable(14, resourceHandle);
        commandList->SetGraphicsRootDescriptorTable(15, samplerHandle);
    }
}

void Device::setRootCbv(ID3D12GraphicsCommandList* commandList, unsigned int idx, ID3D12Resource* buffer, bool bCompute)
{
    auto addr = buffer->GetGPUVirtualAddress();
    if (bCompute)
    {
        commandList->SetComputeRootConstantBufferView(idx, addr);
    }
    else
    {
        commandList->SetGraphicsRootConstantBufferView(idx, addr);
    }
}



