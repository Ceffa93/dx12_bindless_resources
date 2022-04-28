#pragma once
#include "../Shared/Bindless.h"

#define SRV_REGISTER t0
#define UAV_REGISTER u0
#define CBV_REGISTER b0
#define SAMPLER_REGISTER s0
#define UNBOUNDED_ARRAY_NAME(DescriptorNameSuffix) s_unbounded_array_##DescriptorNameSuffix


#pragma region(Unbounded Array Definition)

#define DEFINE_UNBOUNDED_ARRAY(Category, Space, Type, NameSuffix) \
    Type UNBOUNDED_ARRAY_NAME(NameSuffix) [] : register(Category##_REGISTER, space##Space)

#define TYPED_DESCRIPTOR(Category, Space, ObjectType, FormatType)   DEFINE_UNBOUNDED_ARRAY(Category, Space, ObjectType<FormatType>, ObjectType##_##FormatType)
#define UNTYPED_DESCRIPTOR(Category, Space, ObjectType)             DEFINE_UNBOUNDED_ARRAY(Category, Space, ObjectType, ObjectType)
#define SAMPLER_DESCRIPTOR(Space, ObjectType)                       DEFINE_UNBOUNDED_ARRAY(SAMPLER, Space, SamplerState, SamplerState)
#include "../Shared/DescriptorList.h"
#undef TYPED_DESCRIPTOR
#undef UNTYPED_DESCRIPTOR
#undef SAMPLER_DESCRIPTOR

#undef DEFINE_UNBOUNDED_ARRAY

#pragma endregion

#pragma region(Descriptor Handle Struct Definition)

#define DEFINE_DESCRIPTOR_HANDLE(Type, NameSuffix) \
    struct DESCRIPTOR_HANDLE_NAME(NameSuffix) \
    { \
        Type get() { return UNBOUNDED_ARRAY_NAME(NameSuffix)[_unsafe_private_handle]; } \
        unsigned int _unsafe_private_handle; \
    };

#define TYPED_DESCRIPTOR(Category, Space, ObjectType, FormatType)   DEFINE_DESCRIPTOR_HANDLE(ObjectType<FormatType>, ObjectType##_##FormatType)
#define UNTYPED_DESCRIPTOR(Category, Space, ObjectType)             DEFINE_DESCRIPTOR_HANDLE(ObjectType, ObjectType)
#define SAMPLER_DESCRIPTOR(Space, ObjectType)                       DEFINE_DESCRIPTOR_HANDLE(SamplerState, SamplerState)
#include "../Shared/DescriptorList.h"
#undef TYPED_DESCRIPTOR
#undef UNTYPED_DESCRIPTOR
#undef SAMPLER_DESCRIPTOR

#undef DEFINE_DESCRIPTOR_HANDLE

#pragma endregion

