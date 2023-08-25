// D3D Hello World

#pragma once

#include "HWD3DBuffer_DX12.h"

class HWD3DBufferDepthStencil_DX12 : public HWD3DBufferBase_DX12
{
public:

	static HWD3DBufferDepthStencil_DX12* CreateDepthStencil(DXGI_FORMAT InFormat, int InWidth, int InHeight, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider);

protected:

	void InitDepthStencil(DXGI_FORMAT InFormat, int InWidth, int InHeight, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider);
};
