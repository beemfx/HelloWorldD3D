// D3D Hello World

#pragma once

#include "HWD3DTypes.h"
#include <d3d.h>

class HWD3DMesh_DX2
{
private:
	class HWD3DGame_DX2*const m_Game = nullptr;
	D3DEXECUTEBUFFERDESC m_ExecBufferDesc = { };
	IDirect3DExecuteBuffer* m_ExecBuffer = nullptr;

public:
	static HWD3DMesh_DX2* CreateMesh(class HWD3DGame_DX2* InGame, const char* InFilename);
	void Release();

	void Draw();

private:
	HWD3DMesh_DX2(class HWD3DGame_DX2* InGame, const char* InFilename);
	~HWD3DMesh_DX2();

	bool CreateExecBuffer();
};
