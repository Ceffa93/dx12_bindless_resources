#pragma once
#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class DescriptorManager
{
public:
    DescriptorManager(ID3D12Device* device);
    D3D12_GPU_DESCRIPTOR_HANDLE allocateTexture2DUavDescriptor(ID3D12Resource* texture, DXGI_FORMAT format);
    D3D12_GPU_DESCRIPTOR_HANDLE allocateTexture2DSrvDescriptor(ID3D12Resource* texture, DXGI_FORMAT format);
    D3D12_GPU_DESCRIPTOR_HANDLE allocateTexture3DUavDescriptor(ID3D12Resource* texture, DXGI_FORMAT format);
    D3D12_GPU_DESCRIPTOR_HANDLE allocateTexture3DSrvDescriptor(ID3D12Resource* texture, DXGI_FORMAT format);
    D3D12_GPU_DESCRIPTOR_HANDLE allocateSamplerDescriptor(D3D12_SAMPLER_DESC& desc);
    void setHeaps(ID3D12GraphicsCommandList* commandList);

    void setSignature(ID3D12GraphicsCommandList* commandList, bool bCompute);
    void setTables(ID3D12GraphicsCommandList* commandList, bool bCompute);
	
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

        void allocate(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
        {
            cpuHandle.ptr = cpuStart + lastAllocated * descriptorSize;
            gpuHandle.ptr = gpuStart + lastAllocated * descriptorSize;
            lastAllocated++;
        }

	} m_resourceHeap, m_samplerHeap;

};
