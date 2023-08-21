// D3D Hello World

#include "HWD3DTypes.h"
#include <d3d10.h>

#pragma once

class HWD3DRenderState_DX10
{
public:
	
	static HWD3DRenderState_DX10* CreateRenderState(class HWD3DGame* InGame, const char* InVSFile, const char* InPSFile);

protected:
	
	class HWD3DGame_DX10*const m_Game = nullptr;
	ID3D10InputLayout* m_IL = nullptr;
	ID3D10VertexShader* m_VS = nullptr;
	ID3D10PixelShader* m_PS = nullptr;
	ID3D10RasterizerState* m_RS = nullptr;
	ID3D10SamplerState* m_SS = nullptr;
	ID3D10BlendState* m_BS = nullptr;
	ID3D10DepthStencilState* m_DS = nullptr;

public:
	
	void Release();

	void ApplyRenderState();

protected:
	
	HWD3DRenderState_DX10(class HWD3DGame_DX10* InGame, const char* InVSFile, const char* InPSFile);
	~HWD3DRenderState_DX10();
};
