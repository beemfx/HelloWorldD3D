// D3D Hello World

#pragma once

#include "HWD3DTexture.h"
#include <d3d8.h>

class HWD3DTexture_DX9 : public HWD3DTexture
{
protected:
	class HWD3DGame_DX9*const m_Game = nullptr;
	IDirect3DTexture8* m_Texture = nullptr;

public:
	HWD3DTexture_DX9(class HWD3DGame_DX9* InGame);

protected:
	
	virtual ~HWD3DTexture_DX9() override;

public:
	
	void InitTexture();
	virtual void SetTexture() override;
};
