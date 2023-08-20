// D3D Hello World

#include "HWD3DTypes.h"
#include <d3d10.h>

#pragma once

class HWD3DShader_DX10
{
public:
	
	static HWD3DShader_DX10* CreateShader(class HWD3DGame* InGame, const char* InVSFile, const char* InPSFile);

protected:
	
	class HWD3DGame_DX10*const m_Game = nullptr;
	ID3D10InputLayout* m_IL = nullptr;
	ID3D10VertexShader* m_VS = nullptr;
	ID3D10PixelShader* m_PS = nullptr;

public:
	
	void Release();

	void SetShader();

protected:
	
	HWD3DShader_DX10(class HWD3DGame_DX10* InGame, const char* InVSFile, const char* InPSFile);
	~HWD3DShader_DX10();
};
