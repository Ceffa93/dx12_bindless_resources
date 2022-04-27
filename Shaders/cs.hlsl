#include "include.h"

[numthreads(8, 8, 1)]
void CSMain(int2 n : SV_DispatchThreadID)
{
	float power = length(n + -3.5) / 4;
	s_RWTexture2D_float4[flags.uav2D._unsafe_handle][n]			= float4(1, 0, 0, 1) * power;
	s_RWTexture3D_float4[flags.uav3D._unsafe_handle][int3(n,1)]	= float4(0, 0, 1, 1) * power;
}
