// D3D Hello World

#include "DX9_SM3.hlsli"

float4 main(in hwd3d_vertex In) : SV_TARGET
{
	return tex2D(DefaultSampler, In.Tex);
}
