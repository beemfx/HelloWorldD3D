// D3D Hello World

#include "DX9_SM3.hlsli"

hwd3d_vertex main(in hwd3d_vertex In)
{
	In.Pos = mul(In.Pos, WVP);
	return In;
}