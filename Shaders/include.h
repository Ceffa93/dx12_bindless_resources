#include "../Shared/Flags.h"

#define TEX2D_UAV 0 
#define TEX2D_SRV 1 
#define TEX3D_UAV 2
#define TEX3D_SRV 3 

RWTexture2D	<float4> g_rw_texture2D[] : register(u0, space1);
RWTexture3D	<float4> g_rw_texture3D[] : register(u0, space2);
Texture2D g_texture2D[] : register(t0, space1);
Texture3D g_texture3D[] : register(t0, space2);
SamplerState g_sampler[] : register(s0, space1);
