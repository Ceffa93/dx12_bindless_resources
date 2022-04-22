#include "include.h"

[numthreads(8, 8, 1)]
void CSMain(int2 n : SV_DispatchThreadID)
{
	float power = length(n + -3.5) / 4;
	g_rw_texture2D[TEX2D_UAV][n]			= float4(1, 0, 0, 1) * power;
	g_rw_texture3D[TEX3D_UAV][int3(n,1)]	= float4(0, 0, 1, 1) * power;
}
