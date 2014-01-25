#include "Header.hlsli"

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	pos = mul(pos, world);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	return pos;
}