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
	
	if (left) return s_Texture2D[flags.srv2D._unsafe_handle].Sample(s_SamplerState[flags.sampler._unsafe_handle], uv);
	else return s_Texture3D[flags.srv3D._unsafe_handle].Sample(s_SamplerState[flags.sampler._unsafe_handle], float3(uv,1));
}
