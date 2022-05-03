#include "DescriptorManager.h"

template <class Name>
class DescriptorHandle
{
public:
    explicit DescriptorHandle(unsigned int handle = 0)
        : m_handle(handle)
    {}

    unsigned int get() { return m_handle; }

private:
    unsigned int m_handle;
    unsigned int _pad_to_match_hlsl_alignment[3];
};

class Descriptor {
public:
    Descriptor(DescriptorManager& manager) : m_manager(manager) {}

    Descriptor(const Descriptor&) = delete;
    Descriptor& operator=(const Descriptor&) = delete;
    Descriptor(Descriptor&&) = delete; // TODO: can be movable
    Descriptor& operator=(Descriptor&&) = delete; // TODO: can be movable

protected:
    DescriptorManager& m_manager;
};

class Descriptor_RWTexture2D_float4 : private Descriptor {
public:
    Descriptor_RWTexture2D_float4(DescriptorManager& manager) : Descriptor(manager), m_handle(m_manager.allocateResourceDescriptor()) {}
    ~Descriptor_RWTexture2D_float4() { m_manager.deallocateResourceDescriptor(m_handle.get()); }
    using Handle = DescriptorHandle<Descriptor_RWTexture2D_float4>;
    Handle get() { return m_handle; }
    void create(ID3D12Resource* res, DXGI_FORMAT format, D3D12_TEX2D_UAV desc) { m_manager.createTexture2DUavDescriptor(m_handle.get(), res, format, desc); }
private:
    Handle m_handle;
};
class Descriptor_RWTexture3D_float4 : private Descriptor {
public:
    Descriptor_RWTexture3D_float4(DescriptorManager& manager) : Descriptor(manager), m_handle(m_manager.allocateResourceDescriptor()) {}
    ~Descriptor_RWTexture3D_float4() { m_manager.deallocateResourceDescriptor(m_handle.get()); }
    using Handle = DescriptorHandle<Descriptor_RWTexture3D_float4>;
    Handle get() { return m_handle; }
    void create(ID3D12Resource* res, DXGI_FORMAT format, D3D12_TEX3D_UAV desc) { m_manager.createTexture3DUavDescriptor(m_handle.get(), res, format, desc); }
private:
    Handle m_handle;
};
class Descriptor_Texture2D : private Descriptor {
public:
    Descriptor_Texture2D(DescriptorManager& manager) : Descriptor(manager), m_handle(m_manager.allocateResourceDescriptor()) {}
    ~Descriptor_Texture2D() { m_manager.deallocateResourceDescriptor(m_handle.get()); }
    using Handle = DescriptorHandle<Descriptor_Texture2D>;
    Handle get() { return m_handle; }
    void create(ID3D12Resource* res, DXGI_FORMAT format, UINT mapping, D3D12_TEX2D_SRV desc) { m_manager.createTexture2DSrvDescriptor(m_handle.get(), res, format, mapping, desc); }
private:
    Handle m_handle;
};
class Descriptor_Texture3D : private Descriptor {
public:
    Descriptor_Texture3D(DescriptorManager& manager) : Descriptor(manager), m_handle(m_manager.allocateResourceDescriptor()) {}
    ~Descriptor_Texture3D() { m_manager.deallocateResourceDescriptor(m_handle.get()); }
    using Handle = DescriptorHandle<Descriptor_Texture3D>;
    Handle get() { return m_handle; }
    void create(ID3D12Resource* res, DXGI_FORMAT format, UINT mapping, D3D12_TEX3D_SRV desc) { m_manager.createTexture3DSrvDescriptor(m_handle.get(), res, format, mapping, desc); }
private:
    Handle m_handle;
};

class Descriptor_SamplerState : private Descriptor {
public:
    Descriptor_SamplerState(DescriptorManager& manager): Descriptor(manager), m_handle(m_manager.allocateSamplerDescriptor()) {}
    ~Descriptor_SamplerState() { m_manager.deallocateSamplerDescriptor(m_handle.get()); }
    using Handle = DescriptorHandle<Descriptor_SamplerState>;
    Handle get() { return m_handle; }
private:
    Handle m_handle;
};

using DescriptorHandle_RWTexture2D_float4 = Descriptor_RWTexture2D_float4::Handle;
using DescriptorHandle_RWTexture3D_float4 = Descriptor_RWTexture3D_float4::Handle;
using DescriptorHandle_Texture2D = Descriptor_Texture2D::Handle;
using DescriptorHandle_Texture3D = Descriptor_Texture3D::Handle;
using DescriptorHandle_SamplerState = Descriptor_SamplerState::Handle;
