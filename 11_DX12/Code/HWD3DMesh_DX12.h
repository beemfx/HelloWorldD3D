// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include "HWD3DViewProvider_DX12.h"
#include "HWD3DBuffer_DX12.h"
#include <d3d12.h>

class HWD3DMesh_DX12 : public HWD3DMesh
{
private:
	class HWD3DGame_DX12*const m_Game = nullptr;

	HWD3DBuffer_DX12 m_VBBuffer;
	HWD3DBuffer_DX12 m_IBBuffer;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX12(class HWD3DGame_DX12* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX12() override;

protected:
	
	bool CreateBuffers();
};
