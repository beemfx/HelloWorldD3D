// D3D Hello World

#pragma once

#include "HWD3DTexture.h"
#include <d3d.h>

class HWD3DTexture_DX6 : public HWD3DTexture
{
protected:
	class HWD3DGame_DX6*const m_Game = nullptr;
	IDirectDrawSurface4* m_Surface = nullptr;
	IDirect3DTexture2* m_Texture = nullptr;

public:
	HWD3DTexture_DX6(class HWD3DGame_DX6* InGame);

protected:
	
	virtual ~HWD3DTexture_DX6() override;

public:
	
	void InitTexture();
	virtual void SetTexture() override;
};
