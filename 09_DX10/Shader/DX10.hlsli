// D3D Hello World

sampler DefaultSampler : register(s0);
Texture2D Texture : register(t0);
float4x4 WVP : register(c0);

struct hwd3d_vertex
{
	float4 Pos : SV_POSITION;
	float4 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
};
