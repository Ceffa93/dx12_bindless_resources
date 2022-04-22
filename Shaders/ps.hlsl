#include "include.h"

struct PSInput
{
	float4 position 	: SV_POSITION;
	float2 uv 			: TEXCOORD;
};
float4 PSMain(PSInput input) : SV_TARGET
{
	bool left = input.uv.x < .5;
	bool up = input.uv.y > .5;

	float2 uv = fmod(input.uv * 2, 1);

	uint idx;
	if (up)
	{
		if (left) idx = RED_TEX2D_SRV;
		else idx = YELLOW_TEX2D_SRV;
		return g_texture2D[idx].Sample(g_sampler[0], uv);
	}
	else
	{
		if (left) idx = BLUE_TEX3D_SRV;
		else idx = WHITE_TEX3D_SRV;
		return g_texture3D[idx].Sample(g_sampler[0], float3(uv,1));
	}
}
