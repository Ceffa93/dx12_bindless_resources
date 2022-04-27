#pragma once

#if __cplusplus 
#define ALIGN_256 __declspec(align(256))
#else
#define ALIGN_256
#endif

#if __cplusplus
#include "../Source/DescriptorHandle.h"
#define DEFINE_DESCRIPTOR_HANDLE_WITH_FORMAT(Type, Format) using DescriptorHandle_##Type##_##Format = DescriptorHandle<struct _DescriptorHandle_##Type##_##Format>
#define DEFINE_DESCRIPTOR_HANDLE(Type) using DescriptorHandle_##Type = DescriptorHandle<struct _DescriptorHandle_##Type>
DEFINE_DESCRIPTOR_HANDLE_WITH_FORMAT(RWTexture2D, float4);
DEFINE_DESCRIPTOR_HANDLE_WITH_FORMAT(RWTexture3D, float4);
DEFINE_DESCRIPTOR_HANDLE(Texture2D);
DEFINE_DESCRIPTOR_HANDLE(Texture3D);
DEFINE_DESCRIPTOR_HANDLE(SamplerState);

#else
#define DEFINE_DESCRIPTOR_ARRAY_WITH_FORMAT(Type, Format, Register, Space) Type<Format> s_##Type##_##Format [] : register(Register##0, space##Space)
#define DEFINE_DESCRIPTOR_ARRAY(Type, Register, Space) Type s_##Type [] : register(Register##0, space##Space)
DEFINE_DESCRIPTOR_ARRAY_WITH_FORMAT(RWTexture2D, float4, u, 1);
DEFINE_DESCRIPTOR_ARRAY_WITH_FORMAT(RWTexture3D, float4, u, 2);
DEFINE_DESCRIPTOR_ARRAY(Texture2D, t, 1);
DEFINE_DESCRIPTOR_ARRAY(Texture3D, t, 2);
DEFINE_DESCRIPTOR_ARRAY(SamplerState, s, 1);

#define DEFINE_DESCRIPTOR_HANDLE_WITH_FORMAT(Type, Format) struct DescriptorHandle_##Type##_##Format { unsigned int _unsafe_handle; };
#define DEFINE_DESCRIPTOR_HANDLE(Type) struct DescriptorHandle_##Type { unsigned int _unsafe_handle; };
DEFINE_DESCRIPTOR_HANDLE_WITH_FORMAT(RWTexture2D, float4);
DEFINE_DESCRIPTOR_HANDLE_WITH_FORMAT(RWTexture3D, float4);
DEFINE_DESCRIPTOR_HANDLE(Texture2D);
DEFINE_DESCRIPTOR_HANDLE(Texture3D);
DEFINE_DESCRIPTOR_HANDLE(SamplerState);

#endif
