// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include <d3d11.h>

class HWD3DMesh_DX11 : public HWD3DMesh
{
private:
	class HWD3DGame_DX11*const m_Game = nullptr;
	ID3D11Buffer* m_VB = nullptr;
	ID3D11Buffer* m_IB = nullptr;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX11(class HWD3DGame_DX11* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX11() override;

protected:
	
	bool CreateBuffers();
};
