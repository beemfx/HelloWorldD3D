// D3D Hello World

#pragma once

#include "HWD3DTexture.h"
#include <d3d.h>

class HWD3DTexture_DX8 : public HWD3DTexture
{
protected:
	class HWD3DGame_DX8*const m_Game = nullptr;
	IDirectDrawSurface7* m_Surface = nullptr;

public:
	HWD3DTexture_DX8(class HWD3DGame_DX8* InGame);

protected:
	
	virtual ~HWD3DTexture_DX8() override;

public:
	
	void InitTexture();
	virtual void SetTexture() override;
};
