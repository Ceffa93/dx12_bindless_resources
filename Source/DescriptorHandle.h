#pragma once
#include <utility>
#include "../Shared/Bindless.h"

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


#pragma region(Descriptor Handle Named Type Definition)

#define DEFINE_DESCRIPTOR_HANDLE(NameSuffix) \
    using DESCRIPTOR_HANDLE_NAME(NameSuffix) = DescriptorHandle<struct DESCRIPTOR_HANDLE_NAME(NameSuffix)##_Tag>

#define TYPED_DESCRIPTOR(Category, Space, ObjectType, FormatType)   DEFINE_DESCRIPTOR_HANDLE(ObjectType##_##FormatType)
#define UNTYPED_DESCRIPTOR(Category, Space, ObjectType)             DEFINE_DESCRIPTOR_HANDLE(ObjectType)
#define SAMPLER_DESCRIPTOR(Space, ObjectType)                       DEFINE_DESCRIPTOR_HANDLE(ObjectType)
#include "../Shared/DescriptorList.h"
#undef TYPED_DESCRIPTOR
#undef UNTYPED_DESCRIPTOR
#undef SAMPLER_DESCRIPTOR

#undef DEFINE_DESCRIPTOR_HANDLE

#pragma endregion
