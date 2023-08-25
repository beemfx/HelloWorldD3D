// D3D Hello World

#pragma once

#include "HWD3DMesh.h"
#include "HWD3DCore_DX12.h"

class HWD3DMesh_DX12 : public HWD3DMesh
{
private:
	class HWD3DGame_DX12*const m_Game = nullptr;

	class HWD3DBufferConstant_DX12* m_VBBuffer = nullptr;
	class HWD3DBufferConstant_DX12* m_IBBuffer = nullptr;

public:

	virtual void Draw() override;

public:
	
	HWD3DMesh_DX12(class HWD3DGame_DX12* InGame, const char* InFilename);
	virtual ~HWD3DMesh_DX12() override;

protected:
	
	bool CreateBuffers();
};
