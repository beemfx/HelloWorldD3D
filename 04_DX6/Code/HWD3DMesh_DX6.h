// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include <d3d.h>

class HWD3DMesh_DX6 : public HWD3DMesh
{
private:
	class HWD3DGame_DX6*const m_Game = nullptr;
	std::vector<D3DVERTEX> m_D3DVertices;
	std::vector<WORD> m_D3DIndexes;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX6(class HWD3DGame_DX6* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX6() override;

protected:
	
	bool CreateExecBuffer();
};
