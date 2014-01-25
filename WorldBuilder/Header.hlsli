Texture2D txDiffuse : register(t0);
Texture2D txHeightmap : register(t1);
Texture2D txNormalmap : register(t2);

SamplerState samLinear : register(s0);

cbuffer cbProjection : register(b0)
{
	matrix matProjection;
};

cbuffer cbView : register(b1)
{
	matrix matView;
};

cbuffer cbWorld : register(b2)
{
	matrix matWorld;
};

cbuffer cbLights : register(b3)
{
	float4 light_pos[8];
}

struct VS_INPUT
{
	float4 pos : POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD0;
};