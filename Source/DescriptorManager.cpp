#include "DescriptorManager.h"
#include <External/DXSampleHelper.h>
#include <External/d3dx12.h>


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

        CD3DX12_DESCRIPTOR_RANGE1 resourcesRanges[4];
        resourcesRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, -1, 0, 1, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, 0);
        resourcesRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, -1, 0, 2, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, 0);
        resourcesRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, -1, 0, 1, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, 0);
        resourcesRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, -1, 0, 2, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, 0);
        CD3DX12_DESCRIPTOR_RANGE1 samplerRanges[1];
        samplerRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, -1, 0, 1, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0);

        CD3DX12_ROOT_PARAMETER1 rootParameters[16];
        for (UINT i = 0; i < 14; i++)
        {
            rootParameters[i].InitAsConstantBufferView(i, 0u, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
        }
        rootParameters[14].InitAsDescriptorTable(_countof(resourcesRanges), resourcesRanges);
        rootParameters[15].InitAsDescriptorTable(_countof(samplerRanges), samplerRanges);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
        ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
    }
}

DescriptorHandle DescriptorManager::allocateTexture2DUavDescriptor(ID3D12Resource* texture, D3D12_UNORDERED_ACCESS_VIEW_DESC desc)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    DescriptorHandle handle;
    m_resourceHeap.allocate(cpuHandle, handle);
    m_device->CreateUnorderedAccessView(texture, nullptr, &desc, cpuHandle);
    return handle;
}

DescriptorHandle DescriptorManager::allocateTexture2DSrvDescriptor(ID3D12Resource* texture, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    DescriptorHandle handle;
    m_resourceHeap.allocate(cpuHandle, handle);
    m_device->CreateShaderResourceView(texture, &desc, cpuHandle);
    return handle;
}

DescriptorHandle DescriptorManager::allocateTexture3DUavDescriptor(ID3D12Resource* texture, D3D12_UNORDERED_ACCESS_VIEW_DESC desc)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    DescriptorHandle handle;
    m_resourceHeap.allocate(cpuHandle, handle);
    m_device->CreateUnorderedAccessView(texture, nullptr, &desc, cpuHandle);
    return handle;
}

DescriptorHandle DescriptorManager::allocateTexture3DSrvDescriptor(ID3D12Resource* texture, D3D12_SHADER_RESOURCE_VIEW_DESC desc)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    DescriptorHandle handle;
    m_resourceHeap.allocate(cpuHandle, handle);
    m_device->CreateShaderResourceView(texture, &desc, cpuHandle);
    return handle;
}

DescriptorHandle DescriptorManager::allocateSamplerDescriptor(D3D12_SAMPLER_DESC& desc)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    DescriptorHandle handle;
    m_samplerHeap.allocate(cpuHandle, handle);
    m_device->CreateSampler(&desc, cpuHandle);
    return handle;
}

DescriptorHandle DescriptorManager::allocateCbvDescriptor(D3D12_CONSTANT_BUFFER_VIEW_DESC& desc)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    DescriptorHandle handle;
    m_resourceHeap.allocate(cpuHandle, handle);
    m_device->CreateConstantBufferView(&desc, cpuHandle);
    return handle;
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



