// D3D Hello World

#include "DX12.hlsli"

float4 main(in hwd3d_vertex In) : SV_TARGET
{
	return Texture.Sample(DefaultSampler, In.Tex);
}
