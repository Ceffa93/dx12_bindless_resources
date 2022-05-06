#pragma once
#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class DescriptorManager
{
public:
    DescriptorManager(ID3D12Device* device);
    unsigned int allocateResourceDescriptor();
    unsigned int allocateSamplerDescriptor();
    void deallocateResourceDescriptor(unsigned int);
    void deallocateSamplerDescriptor(unsigned int);

    void createTexture2DUavDescriptor(unsigned int handle, ID3D12Resource*, DXGI_FORMAT, D3D12_TEX2D_UAV);
    void createTexture2DSrvDescriptor(unsigned int handle, ID3D12Resource*, DXGI_FORMAT, UINT mapping, D3D12_TEX2D_SRV);
    void createTexture3DUavDescriptor(unsigned int handle, ID3D12Resource*, DXGI_FORMAT, D3D12_TEX3D_UAV);
    void createTexture3DSrvDescriptor(unsigned int handle, ID3D12Resource*, DXGI_FORMAT, UINT mapping, D3D12_TEX3D_SRV);
    void createSamplerDescriptor(unsigned int handle, D3D12_SAMPLER_DESC& desc);
    void setHeaps(ID3D12GraphicsCommandList* commandList);

    void setSignature(ID3D12GraphicsCommandList* commandList, bool bCompute);
    void setTables(ID3D12GraphicsCommandList* commandList, bool bCompute);
    void setRootCbv(ID3D12GraphicsCommandList* commandList, unsigned int idx, ID3D12Resource* buffer, bool bCompute);
	
    ComPtr<ID3D12RootSignature> m_rootSignature;

private:
    ID3D12Device* m_device;
	
	struct Heap
	{
        Heap(ID3D12Device* device, unsigned int descriptorNum, D3D12_DESCRIPTOR_HEAP_TYPE heapType);

        ComPtr<ID3D12DescriptorHeap> heap;
        SIZE_T descriptorSize;
        SIZE_T cpuStart;
        SIZE_T lastAllocated;
        SIZE_T gpuStart;

        D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle(unsigned int handle) { return { cpuStart + handle * descriptorSize }; }

        unsigned int allocate() { return unsigned int(lastAllocated++); }

	} m_resourceHeap, m_samplerHeap;

};
