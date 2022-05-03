#pragma once

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

using DescriptorHandle_RWTexture2D_float4 = DescriptorHandle<struct _DescriptorHandle_RWTexture2D_float4>;
using DescriptorHandle_RWTexture3D_float4 = DescriptorHandle<struct _DescriptorHandle_RWTexture3D_float4>;
using DescriptorHandle_Texture2D = DescriptorHandle<struct _DescriptorHandle_Texture2D>;
using DescriptorHandle_Texture3D = DescriptorHandle<struct _DescriptorHandle_Texture3D>;
using DescriptorHandle_SamplerState = DescriptorHandle<struct _DescriptorHandle_SamplerState>;
