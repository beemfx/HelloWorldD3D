// D3D Hello World

#pragma once

#include "HWD3DTexture.h"
#include <d3d.h>

class HWD3DTexture_DX5 : public HWD3DTexture
{
protected:
	class HWD3DGame_DX5*const m_Game = nullptr;
	IDirectDrawSurface* m_Surface = nullptr;
	IDirect3DTexture2* m_Texture = nullptr;
	D3DTEXTUREHANDLE m_TextureHandle = 0;

public:
	HWD3DTexture_DX5(class HWD3DGame_DX5* InGame);

protected:
	
	virtual ~HWD3DTexture_DX5() override;

public:
	
	void InitTexture();
	virtual void SetTexture() override;
};
