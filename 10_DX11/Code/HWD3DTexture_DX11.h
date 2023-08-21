// D3D Hello World

#pragma once

#include "HWD3DTexture.h"
#include <d3d11.h>

class HWD3DTexture_DX11 : public HWD3DTexture
{
protected:
	class HWD3DGame_DX11*const m_Game = nullptr;
	ID3D11Texture2D* m_Texture = nullptr;
	ID3D11ShaderResourceView* m_View = nullptr;

public:
	HWD3DTexture_DX11(class HWD3DGame_DX11* InGame);

protected:
	
	virtual ~HWD3DTexture_DX11() override;

public:
	
	void InitTexture();
	virtual void SetTexture() override;
};
