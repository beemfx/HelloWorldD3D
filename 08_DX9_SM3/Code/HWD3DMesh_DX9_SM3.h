// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include <d3d9.h>

class HWD3DMesh_DX9_SM3 : public HWD3DMesh
{
private:
	class HWD3DGame_DX9_SM3*const m_Game = nullptr;
	IDirect3DVertexBuffer9* m_VB = nullptr;
	IDirect3DIndexBuffer9* m_IB = nullptr;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX9_SM3(class HWD3DGame_DX9_SM3* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX9_SM3() override;

protected:
	
	bool CreateBuffers();
};
