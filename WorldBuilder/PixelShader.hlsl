#include "header.hlsli"

float4 main(PS_INPUT input) : SV_Target
{
	float4 output = txDiffuse.Sample(samLinear, input.tex);

	//for (unsigned int i = 0; i < 8; i++)
	//{
	//	if (light_pos[i].y < 0.0f)
	//	{
	//		continue;
	//	}

	//	float4 light_vector = normalize(light_pos[i] - input.pos);

	//	output += dot(input.norm, light_vector);
	//}

	return output;
}