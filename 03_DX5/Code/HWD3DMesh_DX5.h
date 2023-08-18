// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include <d3d.h>

class HWD3DMesh_DX5 : public HWD3DMesh
{
private:
	class HWD3DGame_DX5*const m_Game = nullptr;
	D3DEXECUTEBUFFERDESC m_ExecBufferDesc = { };
	IDirect3DExecuteBuffer* m_ExecBuffer = nullptr;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX5(class HWD3DGame_DX5* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX5() override;

protected:
	
	bool CreateExecBuffer();
};
