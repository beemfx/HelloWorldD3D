// D3D Hello World

#pragma once

#include "HWD3DTexture.h"
#include "HWD3DBuffer_DX12.h"
#include <d3d12.h>

class HWD3DTexture_DX12 : public HWD3DTexture
{
protected:
	class HWD3DGame_DX12*const m_Game = nullptr;
	HWD3DBuffer_DX12 m_TextureBuffer;

public:
	HWD3DTexture_DX12(class HWD3DGame_DX12* InGame);

protected:
	
	virtual ~HWD3DTexture_DX12() override;

public:
	
	void InitTexture();
	virtual void SetTexture() override;
};
