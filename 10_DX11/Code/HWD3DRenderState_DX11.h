// D3D Hello World

#include "HWD3DTypes.h"
#include <d3d11.h>

#pragma once

class HWD3DRenderState_DX11
{
public:
	
	static HWD3DRenderState_DX11* CreateRenderState(class HWD3DGame* InGame, const char* InVSFile, const char* InPSFile);

protected:
	
	class HWD3DGame_DX11*const m_Game = nullptr;
	ID3D11InputLayout* m_IL = nullptr;
	ID3D11VertexShader* m_VS = nullptr;
	ID3D11PixelShader* m_PS = nullptr;
	ID3D11RasterizerState* m_RS = nullptr;
	ID3D11SamplerState* m_SS = nullptr;
	ID3D11BlendState* m_BS = nullptr;
	ID3D11DepthStencilState* m_DS = nullptr;

public:
	
	void Release();

	void ApplyRenderState();

protected:
	
	HWD3DRenderState_DX11(class HWD3DGame_DX11* InGame, const char* InVSFile, const char* InPSFile);
	~HWD3DRenderState_DX11();
};
