#include "DescriptorManager.h"
#include <External/DXSampleHelper.h>
#include <External/d3dx12.h>
#include <array>

DescriptorManager::DescriptorManager(ID3D12Device* device)
{
    m_device = device;
	
	// Create heaps
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 8;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_resourceHeap.heap)));

        m_resourceHeap.cpuStart = m_resourceHeap.heap->GetCPUDescriptorHandleForHeapStart().ptr;
        m_resourceHeap.gpuStart = m_resourceHeap.heap->GetGPUDescriptorHandleForHeapStart().ptr;
        m_resourceHeap.descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        m_resourceHeap.lastAllocated = 0;
    }
	{
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 1;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_samplerHeap.heap)));

        m_samplerHeap.cpuStart = m_samplerHeap.heap->GetCPUDescriptorHandleForHeapStart().ptr;
        m_samplerHeap.gpuStart = m_samplerHeap.heap->GetGPUDescriptorHandleForHeapStart().ptr;
        m_samplerHeap.descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        m_samplerHeap.lastAllocated = 0;
	}

	
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

unsigned DescriptorManager::allocateResourceDescriptor()
{
    // TODO: don't need cpu handle
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    unsigned int handle;
    m_resourceHeap.allocate(cpuHandle, handle);
    return handle;
}
unsigned DescriptorManager::allocateSamplerDescriptor()
{
    // TODO: don't need cpu handle
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    unsigned int handle;
    m_samplerHeap.allocate(cpuHandle, handle);
    return handle;
}
void DescriptorManager::deallocateResourceDescriptor(unsigned int)
{
    // TODO: stack allocator does not support deallocate
}
void DescriptorManager::deallocateSamplerDescriptor(unsigned int)
{
    // TODO: stack allocator does not support deallocate
}

void DescriptorManager::createTexture2DUavDescriptor(unsigned int handle, ID3D12Resource* texture, DXGI_FORMAT format, D3D12_TEX2D_UAV resDesc)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
    desc.Format = format;
    desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    desc.Texture2D = resDesc;

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_resourceHeap.cpuStart + handle * m_resourceHeap.descriptorSize };
    m_device->CreateUnorderedAccessView(texture, nullptr, &desc, cpuHandle);
}
void DescriptorManager::createTexture2DSrvDescriptor(unsigned int handle, ID3D12Resource* texture, DXGI_FORMAT format, UINT mapping, D3D12_TEX2D_SRV resDesc)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = format;
    desc.Shader4ComponentMapping = mapping;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D = resDesc;

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_resourceHeap.cpuStart + handle * m_resourceHeap.descriptorSize };
    m_device->CreateShaderResourceView(texture, &desc, cpuHandle);
}
void DescriptorManager::createTexture3DUavDescriptor(unsigned int handle, ID3D12Resource* texture, DXGI_FORMAT format, D3D12_TEX3D_UAV resDesc)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
    desc.Format = format;
    desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
    desc.Texture3D = resDesc;

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_resourceHeap.cpuStart + handle * m_resourceHeap.descriptorSize };
    m_device->CreateUnorderedAccessView(texture, nullptr, &desc, cpuHandle);
}
void DescriptorManager::createTexture3DSrvDescriptor(unsigned int handle, ID3D12Resource* texture, DXGI_FORMAT format, UINT mapping, D3D12_TEX3D_SRV resDesc)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = format;
    desc.Shader4ComponentMapping = mapping;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
    desc.Texture3D = resDesc;

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_resourceHeap.cpuStart + handle * m_resourceHeap.descriptorSize };
    m_device->CreateShaderResourceView(texture, &desc, cpuHandle);
}

void DescriptorManager::createSamplerDescriptor(unsigned int handle, D3D12_SAMPLER_DESC& desc)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_samplerHeap.cpuStart + handle * m_samplerHeap.descriptorSize };
    m_device->CreateSampler(&desc, cpuHandle);
}

void DescriptorManager::createCbvDescriptor(unsigned int handle, D3D12_CONSTANT_BUFFER_VIEW_DESC& desc)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_resourceHeap.cpuStart + handle * m_resourceHeap.descriptorSize };
    m_device->CreateConstantBufferView(&desc, cpuHandle);
}

void DescriptorManager::setHeaps(ID3D12GraphicsCommandList* commandList)
{
    ID3D12DescriptorHeap* ppHeaps[] = { m_resourceHeap.heap.Get(), m_samplerHeap.heap.Get() };
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void DescriptorManager::setSignature(ID3D12GraphicsCommandList* commandList, bool bCompute)
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

void DescriptorManager::setTables(ID3D12GraphicsCommandList* commandList, bool bCompute)
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

void DescriptorManager::setRootCbv(ID3D12GraphicsCommandList* commandList, unsigned int idx, ID3D12Resource* buffer, bool bCompute)
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



