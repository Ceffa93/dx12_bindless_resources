#include "include.h"

struct PSInput
{
	float4 position 	: SV_POSITION;
	float2 uv 			: TEXCOORD;
};
float4 PSMain(PSInput input) : SV_TARGET
{
	bool left = input.uv.x < .5;

	float2 uv = float2(frac(input.uv.x * 2), input.uv.y);
	
	if (left) return g_texture2D[flags.srv2D].Sample(g_sampler[flags.sampler], uv);
	else return g_texture3D[flags.srv3D].Sample(g_sampler[flags.sampler], float3(uv,1));
}
