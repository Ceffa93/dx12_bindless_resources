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
	
	if (left) return flags.srv2D.get().Sample(flags.sampler.get(), uv);
	else return flags.srv3D.get().Sample(flags.sampler.get(), float3(uv, 1));
}

