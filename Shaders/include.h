#include "../Shared/Flags.h"

cbuffer Buffer0 : register(b0, space0)
{
    Flags flags;
}

RWTexture2D	<float4> g_rw_texture2D[] : register(u0, space1);
RWTexture3D	<float4> g_rw_texture3D[] : register(u0, space2);
Texture2D g_texture2D[] : register(t0, space1);
Texture3D g_texture3D[] : register(t0, space2);
SamplerState g_sampler[] : register(s0, space1);
