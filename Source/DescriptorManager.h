#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "DescriptorHandle.h"

using Microsoft::WRL::ComPtr;

class DescriptorManager
{
public:
    DescriptorManager(ID3D12Device* device);
    unsigned int allocateTexture2DUavDescriptor(ID3D12Resource* texture, D3D12_UNORDERED_ACCESS_VIEW_DESC);
    unsigned int allocateTexture2DSrvDescriptor(ID3D12Resource* texture, D3D12_SHADER_RESOURCE_VIEW_DESC);
    unsigned int allocateTexture3DUavDescriptor(ID3D12Resource* texture, D3D12_UNORDERED_ACCESS_VIEW_DESC);
    unsigned int allocateTexture3DSrvDescriptor(ID3D12Resource* texture, D3D12_SHADER_RESOURCE_VIEW_DESC);
    unsigned int allocateSamplerDescriptor(D3D12_SAMPLER_DESC& desc);
    unsigned int allocateCbvDescriptor(D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);
    void setHeaps(ID3D12GraphicsCommandList* commandList);

    void setSignature(ID3D12GraphicsCommandList* commandList, bool bCompute);
    void setTables(ID3D12GraphicsCommandList* commandList, bool bCompute);
    void setRootCbv(ID3D12GraphicsCommandList* commandList, unsigned int idx, ID3D12Resource* buffer, bool bCompute);
	
    ComPtr<ID3D12RootSignature> m_rootSignature;

private:
    ID3D12Device* m_device;
	
	struct Heap
	{
        ComPtr<ID3D12DescriptorHeap> heap;
        SIZE_T descriptorSize;
        SIZE_T cpuStart;
        SIZE_T lastAllocated;
        SIZE_T gpuStart;

        void allocate(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, unsigned int& handle)
        {
            handle = unsigned int(lastAllocated);
            cpuHandle.ptr = cpuStart + lastAllocated * descriptorSize;
            lastAllocated++;
        }

	} m_resourceHeap, m_samplerHeap;

};
