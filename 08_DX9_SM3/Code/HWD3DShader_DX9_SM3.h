// D3D Hello World

#include "HWD3DTypes.h"
#include <d3d9.h>

#pragma once

class HWD3DShader_DX9_SM3
{
public:
	
	static HWD3DShader_DX9_SM3* CreateShader(class HWD3DGame* InGame, const char* InVSFile, const char* InPSFile);

protected:
	
	class HWD3DGame_DX9_SM3*const m_Game = nullptr;
	IDirect3DVertexDeclaration9* m_VD = nullptr;
	IDirect3DVertexShader9* m_VS = nullptr;
	IDirect3DPixelShader9* m_PS = nullptr;

public:
	
	void Release();

	void SetShader();

protected:
	
	HWD3DShader_DX9_SM3(class HWD3DGame_DX9_SM3* InGame, const char* InVSFile, const char* InPSFile);
	~HWD3DShader_DX9_SM3();
};
