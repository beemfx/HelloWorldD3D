// D3D Hello World

#include "HWD3DObject.h"
#include "HWD3DTypes.h"
#include <d3d12.h>

#pragma once

class HWD3DRenderState_DX12 : public HWD3DObject
{
public:
	
	static HWD3DRenderState_DX12* CreateRenderState(class HWD3DGame* InGame, const char* InVSFile, const char* InPSFile);

protected:
	
	class HWD3DGame_DX12*const m_Game = nullptr;
	ID3D12PipelineState* m_State = nullptr;

public:
	
	void Release();

	void ApplyRenderState();

protected:
	
	HWD3DRenderState_DX12(class HWD3DGame_DX12* InGame, const char* InVSFile, const char* InPSFile);
	~HWD3DRenderState_DX12();
};
