// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include <d3d9.h>

class HWD3DMesh_DX9 : public HWD3DMesh
{
private:
	class HWD3DGame_DX9*const m_Game = nullptr;
	const DWORD m_FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	IDirect3DVertexBuffer9* m_VB = nullptr;
	IDirect3DIndexBuffer9* m_IB = nullptr;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX9(class HWD3DGame_DX9* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX9() override;

protected:
	
	bool CreateBuffers();
};
