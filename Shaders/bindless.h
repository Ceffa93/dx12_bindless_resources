#pragma once
#include "../Shared/Bindless.h"

RWTexture2D<float4> s_array_RWTexture2D_float4[] : register(u0, space1);
RWTexture3D<float4> s_array_RWTexture3D_float4[] : register(u0, space2);
Texture2D s_array_Texture2D[] : register(t0, space1);
Texture3D s_array_Texture3D[] : register(t0, space2);
SamplerState s_array_SamplerState[] : register(s0, space1);


struct DescriptorHandle_RWTexture2D_float4 { 
    RWTexture2D<float4> get() { return s_array_RWTexture2D_float4[_private_handle]; }
    unsigned int _private_handle;
};
struct DescriptorHandle_RWTexture3D_float4 {
    RWTexture3D<float4> get() { return s_array_RWTexture3D_float4[_private_handle]; }
    unsigned int _private_handle;
};
struct DescriptorHandle_Texture2D {
    Texture2D get() { return s_array_Texture2D[_private_handle]; }
    unsigned int _private_handle;
};
struct DescriptorHandle_Texture3D {
    Texture3D get() { return s_array_Texture3D[_private_handle]; }
    unsigned int _private_handle;
};
struct DescriptorHandle_SamplerState {
    SamplerState get() { return s_array_SamplerState[_private_handle]; }
    unsigned int _private_handle;
};

