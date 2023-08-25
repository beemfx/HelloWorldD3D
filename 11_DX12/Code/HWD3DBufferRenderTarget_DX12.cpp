// D3D Hello World

#include "HWD3DBufferRenderTarget_DX12.h"
#include "HWD3DViewProvider_DX12.h"

HWD3DBufferRenderTarget_DX12* HWD3DBufferRenderTarget_DX12::CreateRenderTarget(IDXGISwapChain1& InSwapChain, UINT InBackBufferIndex, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider)
{
	HWD3DBufferRenderTarget_DX12* Out = new HWD3DBufferRenderTarget_DX12;
	if (Out)
	{
		Out->InitRenderTarget(InSwapChain, InBackBufferIndex, InDevice, InViewProvider);
	}
	return Out;
}

void HWD3DBufferRenderTarget_DX12::InitRenderTarget(IDXGISwapChain1& InSwapChain, UINT InBackBufferIndex, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider)
{
	const HRESULT GetBufferRes = InSwapChain.GetBuffer(InBackBufferIndex, IID_PPV_ARGS(&m_GpuBuffer));
	if (FAILED(GetBufferRes) || !m_GpuBuffer)
	{
		return;
	}

	// Hold on to a copy of our view provider so we can destroy our view in ~HWD3DBufferBase_DX12
	m_ViewProvider = &InViewProvider;
	m_ViewProvider->AddRef();

	m_GpuBufferState = D3D12_RESOURCE_STATE_PRESENT; // Back buffers start in Present state.
	m_GpuView = InViewProvider.CreateView();
	if (!m_GpuView.IsValid())
	{
		return;
	}
	InDevice.CreateRenderTargetView(m_GpuBuffer, nullptr, m_GpuView.CpuDescHandle);
}
