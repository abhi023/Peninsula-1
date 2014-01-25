#include "Header.hlsli"

float4 main(PS_INPUT input) : SV_Target
{
	float4 final;

	final = txDiffuse.Sample(samLinear, input.tex) * colDiffuse;

	float4 opacity = txOpacity.Sample(samLinear, input.tex);
	final[3] = opacity[3];

	return final;
}