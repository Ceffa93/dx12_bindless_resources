#include "DescriptorHandle.h"
#include "DescriptorManager.h"

template <class Handle>
class ResourceDescriptorOwner
{
public:
    ResourceDescriptorOwner(DescriptorManager& manager)
        : m_manager(manager)
        , m_handle(m_manager.allocateResourceDescriptor())
    {}

    ~ResourceDescriptorOwner() { m_manager.deallocateResourceDescriptor(m_handle.get()); }

    ResourceDescriptorOwner(const ResourceDescriptorOwner&) = delete;
    ResourceDescriptorOwner& operator=(const ResourceDescriptorOwner&) = delete;
    ResourceDescriptorOwner(ResourceDescriptorOwner&&) = delete; // TODO: can be movable
    ResourceDescriptorOwner& operator=(ResourceDescriptorOwner&&) = delete; // TODO: can be movable

    Handle get() { return m_handle; }

private:
    DescriptorManager& m_manager;
    Handle m_handle;
};


template <class Handle>
class SamplerDescriptorOwner
{
public:
    SamplerDescriptorOwner(DescriptorManager& manager)
        : m_manager(manager)
        , m_handle(m_manager.allocateSamplerDescriptor())
    {}

    ~SamplerDescriptorOwner() { m_manager.deallocateSamplerDescriptor(m_handle.get()); }

    SamplerDescriptorOwner(const SamplerDescriptorOwner&) = delete;
    SamplerDescriptorOwner& operator=(const SamplerDescriptorOwner&) = delete;
    SamplerDescriptorOwner(SamplerDescriptorOwner&&) = delete; // TODO: can be movable
    SamplerDescriptorOwner& operator=(SamplerDescriptorOwner&&) = delete; // TODO: can be movable

    Handle get() { return m_handle; }

private:
    DescriptorManager& m_manager;
    Handle m_handle;
};

