// D3D Hello World

#pragma once

#include "HWD3DBuffer_DX12.h"

class HWD3DBufferTexture_DX12 : public HWD3DBufferBase_DX12
{
private:
	
	hwd3dTextureFormat m_TextureFormat;

public:

	static HWD3DBufferTexture_DX12* CreateTexture(ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider, const hwd3dTextureFormat& InTextureFormat);

public:
	
	void PrepareForDraw(ID3D12GraphicsCommandList& Context);

protected:

	void InitTexture(ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider, const hwd3dTextureFormat& InTextureFormat);
};
