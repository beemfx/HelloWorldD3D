// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include <d3d.h>

class HWD3DMesh_DX6 : public HWD3DMesh
{
private:
	class HWD3DGame_DX6*const m_Game = nullptr;
	const DWORD m_FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	IDirect3DVertexBuffer* m_VB = nullptr;
	std::vector<WORD> m_D3DIndexes;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX6(class HWD3DGame_DX6* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX6() override;

protected:
	
	bool CreateBuffers();
};
