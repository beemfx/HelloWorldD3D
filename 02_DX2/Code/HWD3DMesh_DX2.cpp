// D3D Hello World

#include "HWD3DMesh_DX2.h"

HWD3DMesh_DX2* HWD3DMesh_DX2::CreateMesh(class HWD3DGame_DX2* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX2(InGame, InFilename);
}

void HWD3DMesh_DX2::Release()
{
	delete this;
}

HWD3DMesh_DX2::HWD3DMesh_DX2(class HWD3DGame_DX2* InGame, const char* InFilename)
{
	
}


HWD3DMesh_DX2::~HWD3DMesh_DX2()
{
	
}
