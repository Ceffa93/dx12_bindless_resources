#pragma once

#if __cplusplus 
#define ALIGN_256 __declspec(align(256))
#else
#define ALIGN_256
#endif

#if __cplusplus
#include "../Source/DescriptorHandle.h"

#define PROCESS_TYPED_DESCRIPTOR(Category, ObjectType, FormatType, Space) using DescriptorHandle_##ObjectType##_##FormatType = DescriptorHandle<struct _DescriptorHandle_##Type##_##Format>
#define PROCESS_UNTYPED_DESCRIPTOR(Category, ObjectType, Space) using DescriptorHandle_##ObjectType = DescriptorHandle<struct _DescriptorHandle_##Type>
#include "DescriptorList.h"
#undef PROCESS_TYPED_DESCRIPTOR
#undef PROCESS_UNTYPED_DESCRIPTOR
#endif
