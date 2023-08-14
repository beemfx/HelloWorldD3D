// D3D Hello World

#pragma once

class HWD3DMesh_DX2
{
public:
	static HWD3DMesh_DX2* CreateMesh(class HWD3DGame_DX2* InGame, const char* InFilename);
	void Release();

private:
	HWD3DMesh_DX2(class HWD3DGame_DX2* InGame, const char* InFilename);
	~HWD3DMesh_DX2();
};
