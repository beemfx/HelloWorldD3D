// D3D Hello World

#pragma once

#include "HWD3DTexture.h"
#include <d3d.h>

class HWD3DTexture_DX2 : public HWD3DTexture
{
protected:
	class HWD3DGame_DX2*const m_Game = nullptr;
	IDirectDrawSurface* m_Surface = nullptr;
	IDirect3DTexture* m_Texture = nullptr;

public:
	HWD3DTexture_DX2(class HWD3DGame_DX2* InGame);

protected:
	
	virtual ~HWD3DTexture_DX2() override;

public:
	
	void InitTexture();
};
