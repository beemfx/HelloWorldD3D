// D3D Hello World

sampler DefaultSampler : register(ps, s0);
Texture2D Texture : register(ps, t0);
cbuffer Consts : register(b0)
{
	float4x4 WVP;
}

struct hwd3d_vertex
{
	float4 Pos : SV_POSITION;
	float4 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};
