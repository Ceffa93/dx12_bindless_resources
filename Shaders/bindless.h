#pragma once

// Utils
#define SRV_REGISTER t0
#define UAV_REGISTER u0
#define CBV_REGISTER b0
#define SAMPLER_REGISTER s0
#define UNBOUNDED_TYPED_ARRAY_NAME(ObjectType, FormatType) s_unbounded_array_##ObjectType##_##FormatType
#define UNBOUNDED_UNTYPED_ARRAY_NAME(ObjectType) s_unbounded_array_##ObjectType
#define UNBOUNDED_TYPED_ARRAY_TYPE(ObjectType, FormatType) ObjectType<FormatType>
#define UNBOUNDED_UNTYPED_ARRAY_TYPE(ObjectType) ObjectType
#define HANDLE_TYPED_STRUCT_NAME(ObjectType, FormatType) DescriptorHandle_##ObjectType##_##FormatType
#define HANDLE_UNTYPED_STRUCT_NAME(ObjectType) DescriptorHandle_##ObjectType

// Definition of unbounded arrays
#define DEFINE_UNBOUNDED_ARRAY(ArrayType, ArrayName, Category, Space) \
    ArrayType ArrayName [] : register(Category##_REGISTER, space##Space)

#define PROCESS_TYPED_DESCRIPTOR(Category, ObjectType, FormatType, Space) \
    DEFINE_UNBOUNDED_ARRAY(UNBOUNDED_TYPED_ARRAY_TYPE(ObjectType, FormatType), UNBOUNDED_TYPED_ARRAY_NAME(ObjectType, FormatType), Category, Space)
#define PROCESS_UNTYPED_DESCRIPTOR(Category, ObjectType, Space) \
    DEFINE_UNBOUNDED_ARRAY(UNBOUNDED_UNTYPED_ARRAY_TYPE(ObjectType), UNBOUNDED_UNTYPED_ARRAY_NAME(ObjectType), Category, Space)
#include "../Shared/DescriptorList.h"
#undef PROCESS_TYPED_DESCRIPTOR
#undef PROCESS_UNTYPED_DESCRIPTOR


// Definition of descriptor handle structs
#define DESCRIPTOR_HANDLE_STRUCT(StructName, DescriptorType, ArrayName) \
    struct StructName \
    { \
        DescriptorType get() { return ArrayName[_unsafe_private_handle]; } \
        unsigned int _unsafe_private_handle; \
    };

#define PROCESS_TYPED_DESCRIPTOR(Category, ObjectType, FormatType, Space) \
    DESCRIPTOR_HANDLE_STRUCT(HANDLE_TYPED_STRUCT_NAME(ObjectType, FormatType), UNBOUNDED_TYPED_ARRAY_TYPE(ObjectType, FormatType), UNBOUNDED_TYPED_ARRAY_NAME(ObjectType, FormatType))
#define PROCESS_UNTYPED_DESCRIPTOR(Category, ObjectType, Space) \
    DESCRIPTOR_HANDLE_STRUCT(HANDLE_UNTYPED_STRUCT_NAME(ObjectType), UNBOUNDED_UNTYPED_ARRAY_TYPE(ObjectType), UNBOUNDED_UNTYPED_ARRAY_NAME(ObjectType))
#include "../Shared/DescriptorList.h"
#undef PROCESS_TYPED_DESCRIPTOR
#undef PROCESS_UNTYPED_DESCRIPTOR
