// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include <d3d8.h>

class HWD3DMesh_DX8 : public HWD3DMesh
{
private:
	class HWD3DGame_DX8*const m_Game = nullptr;
	const DWORD m_FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	IDirect3DVertexBuffer8* m_VB = nullptr;
	IDirect3DIndexBuffer8* m_IB = nullptr;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX8(class HWD3DGame_DX8* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX8() override;

protected:
	
	bool CreateBuffers();
};
