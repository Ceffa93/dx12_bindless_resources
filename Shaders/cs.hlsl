#include "include.h"

[numthreads(8, 8, 1)]
void CSMain(int2 n : SV_DispatchThreadID)
{
	float power = length(n + -3.5) / 4;

	RWTexture2D<float4> t2d = flags.uav2D.get();
	t2d[n] = float4(1, 0, 0, 1) * power;
	RWTexture3D<float4> t3d = flags.uav3D.get();
	t3d[int3(n,1)] = float4(0, 0, 1, 1) * power;
}
