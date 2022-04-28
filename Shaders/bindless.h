#pragma once
#include "../Shared/Bindless.h"

// Utils
#define SRV_REGISTER t0
#define UAV_REGISTER u0
#define CBV_REGISTER b0
#define SAMPLER_REGISTER s0
#define UNBOUNDED_ARRAY_NAME(DescriptorNameSuffix) s_unbounded_array_##DescriptorNameSuffix

#define PROCESS_TYPED_DESCRIPTOR(Category, Space, ObjectType, FormatType) \
    PROCESS_DESCRIPTOR(Category, Space, ObjectType<FormatType>, ObjectType##_##FormatType)
#define PROCESS_UNTYPED_DESCRIPTOR(Category, Space, ObjectType) \
    PROCESS_DESCRIPTOR(Category, Space, ObjectType, ObjectType)
#define PROCESS_SAMPLER_DESCRIPTOR(Space, ObjectType) \
    PROCESS_DESCRIPTOR(SAMPLER, Space, SamplerState, SamplerState)


// Definition of unbounded arrays
#define PROCESS_DESCRIPTOR(Category, Space, Type, NameSuffix) \
    Type UNBOUNDED_ARRAY_NAME(NameSuffix) [] : register(Category##_REGISTER, space##Space)
#include "../Shared/DescriptorList.h"
#undef PROCESS_DESCRIPTOR


// Definition of descriptor handle structs
#define PROCESS_DESCRIPTOR(Category, Space, Type, NameSuffix) \
    struct DESCRIPTOR_HANDLE_NAME(NameSuffix) \
    { \
        Type get() { return UNBOUNDED_ARRAY_NAME(NameSuffix)[_unsafe_private_handle]; } \
        unsigned int _unsafe_private_handle; \
    };
#include "../Shared/DescriptorList.h"
#undef PROCESS_DESCRIPTOR
