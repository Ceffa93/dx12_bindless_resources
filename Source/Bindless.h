#include "../Shared/Bindless.h"
#include "../Source/DescriptorHandle.h"

#define PROCESS_TYPED_DESCRIPTOR(Category, Space, ObjectType, FormatType) \
    PROCESS_DESCRIPTOR(ObjectType##_##FormatType)
#define PROCESS_UNTYPED_DESCRIPTOR(Category, Space, ObjectType) \
    PROCESS_DESCRIPTOR(ObjectType)
#define PROCESS_SAMPLER_DESCRIPTOR(Space, ObjectType) \
    PROCESS_DESCRIPTOR(ObjectType)

#define PROCESS_DESCRIPTOR(NameSuffix) \
    using DESCRIPTOR_HANDLE_NAME(NameSuffix) = DescriptorHandle<struct DESCRIPTOR_HANDLE_NAME(NameSuffix)##_Tag>
#include "../Shared/DescriptorList.h"
#undef PROCESS_DESCRIPTOR