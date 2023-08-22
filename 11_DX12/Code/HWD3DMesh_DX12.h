// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include <d3d12.h>

class HWD3DMesh_DX12 : public HWD3DMesh
{
private:
	class HWD3DGame_DX12*const m_Game = nullptr;
	// ID3D11Buffer* m_VB = nullptr;
	// ID3D11Buffer* m_IB = nullptr;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX12(class HWD3DGame_DX12* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX12() override;

protected:
	
	bool CreateBuffers();
};
