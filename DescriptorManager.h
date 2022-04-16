using Microsoft::WRL::ComPtr;

class DescriptorManager
{
public:
    void init(ID3D12Device* device);
    void allocateTexture2DUavDescriptor(ID3D12Resource* texture, DXGI_FORMAT format);
    void allocateTexture2DSrvDescriptor(ID3D12Resource* texture, DXGI_FORMAT format);
    void allocateTexture3DUavDescriptor(ID3D12Resource* texture, DXGI_FORMAT format);
    void allocateTexture3DSrvDescriptor(ID3D12Resource* texture, DXGI_FORMAT format);
    void allocateSamplerDescriptor(D3D12_SAMPLER_DESC& desc);
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
        SIZE_T start;
        SIZE_T offset = 0;
        SIZE_T table;
	} m_resourceHeap, m_samplerHeap;

};
