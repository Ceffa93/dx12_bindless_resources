#include "include.h"

static const float4 FAST_FULLSCREEN_VERTEX_POS[3] = { float4(-1, 1, 1, 1), float4(3, 1, 1, 1), float4(-1, -3, 1, 1) };
static const float2 FAST_FULLSCREEN_VERTEX_UV[3] = { float2(0, 0), float2(2, 0), float2(0, 2) };

struct PSInput
{
	float4 position 	: SV_POSITION;
	float2 uv 			: TEXCOORD;
};

PSInput VSMain(uint vertexId : SV_VertexID)
{
	PSInput output;
	output.position = FAST_FULLSCREEN_VERTEX_POS[vertexId];
	output.uv = FAST_FULLSCREEN_VERTEX_UV[vertexId];
	return output;
}
