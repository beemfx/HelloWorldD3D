// D3D Hello World

#pragma once

#include "HWD3DTexture.h"
#include <d3d.h>

class HWD3DTexture_DX7 : public HWD3DTexture
{
protected:
	class HWD3DGame_DX7*const m_Game = nullptr;
	IDirectDrawSurface4* m_Surface = nullptr;
	IDirect3DTexture2* m_Texture = nullptr;

public:
	HWD3DTexture_DX7(class HWD3DGame_DX7* InGame);

protected:
	
	virtual ~HWD3DTexture_DX7() override;

public:
	
	void InitTexture();
	virtual void SetTexture() override;
};
