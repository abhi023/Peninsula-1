#define MAX_MATERIALS 256
Texture2D txDiffuse : register(t0);
Texture2D txNormals : register(t1);
Texture2D txOpacity : register(t2);

SamplerState samLinear : register(s0);

cbuffer cbNeverChanges : register(b0)
{
	matrix view;
};

cbuffer cbChangeOnResize : register(b1)
{
	matrix projection;
};

cbuffer cbChangesEveryFrame : register(b2)
{
	matrix world;
	float4 vMeshColour;
};

cbuffer cbMaterial : register(b3)
{
	float4 colDiffuse;
	float4 colTransparent;
};

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