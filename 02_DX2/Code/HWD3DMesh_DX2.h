// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include <d3d.h>

class HWD3DMesh_DX2 : public HWD3DMesh
{
private:
	class HWD3DGame_DX2*const m_Game = nullptr;
	D3DEXECUTEBUFFERDESC m_ExecBufferDesc = { };
	IDirect3DExecuteBuffer* m_ExecBuffer = nullptr;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX2(class HWD3DGame_DX2* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX2() override;

protected:
	
	bool CreateExecBuffer();
};
