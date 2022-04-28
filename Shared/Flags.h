#include "Bindless.h"

struct Flags
{
    DescriptorHandle_Texture2D srv2D;
    DescriptorHandle_Texture3D srv3D;
    DescriptorHandle_RWTexture2D_float4 uav2D;
    DescriptorHandle_RWTexture3D_float4 uav3D;
    DescriptorHandle_SamplerState sampler;
};