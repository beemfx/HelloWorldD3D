// D3D Hello World

#pragma once

#include "HWD3DBuffer_DX12.h"

class HWD3DBufferRenderTarget_DX12 : public HWD3DBufferBase_DX12
{
public:

	static HWD3DBufferRenderTarget_DX12* CreateRenderTarget(IDXGISwapChain4& InSwapChain, UINT BackBufferIndex, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider);

protected:

	void InitRenderTarget(IDXGISwapChain4& InSwapChain, UINT InBackBufferIndex, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider);
};
