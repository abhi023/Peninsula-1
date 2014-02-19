#include "Header.hlsli"

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.pos = mul(input.pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	
	output.tex = input.tex;
	
	output.norm = input.norm;

	return output;
}