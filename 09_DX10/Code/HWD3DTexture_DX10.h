// D3D Hello World

#pragma once

#include "HWD3DTexture.h"
#include <d3d9.h>

class HWD3DTexture_DX10 : public HWD3DTexture
{
protected:
	class HWD3DGame_DX10*const m_Game = nullptr;
	IDirect3DTexture9* m_Texture = nullptr;

public:
	HWD3DTexture_DX10(class HWD3DGame_DX10* InGame);

protected:
	
	virtual ~HWD3DTexture_DX10() override;

public:
	
	void InitTexture();
	virtual void SetTexture() override;
};
