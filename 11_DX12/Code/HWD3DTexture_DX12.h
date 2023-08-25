// D3D Hello World

#pragma once

#include "HWD3DTexture.h"
#include "HWD3DCore_DX12.h"

class HWD3DTexture_DX12 : public HWD3DTexture
{
protected:
	class HWD3DGame_DX12*const m_Game = nullptr;
	class HWD3DBufferTexture_DX12* m_TextureBuffer = nullptr;

public:
	HWD3DTexture_DX12(class HWD3DGame_DX12* InGame);

protected:
	
	virtual ~HWD3DTexture_DX12() override;

public:
	
	void InitTexture();
	virtual void SetTexture() override;
};
