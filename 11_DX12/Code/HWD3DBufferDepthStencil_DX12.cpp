// D3D Hello World

#include "HWD3DBufferDepthStencil_DX12.h"
#include "HWD3DViewProvider_DX12.h"

HWD3DBufferDepthStencil_DX12* HWD3DBufferDepthStencil_DX12::CreateDepthStencil(DXGI_FORMAT InFormat, int InWidth, int InHeight, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider)
{
	HWD3DBufferDepthStencil_DX12* Out = new HWD3DBufferDepthStencil_DX12;
	if (Out)
	{
		Out->InitDepthStencil(InFormat, InWidth, InHeight, InDevice, InViewProvider);
	}
	return Out;
}

void HWD3DBufferDepthStencil_DX12::InitDepthStencil(DXGI_FORMAT InFormat, int InWidth, int InHeight, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider)
{
	D3D12_HEAP_PROPERTIES HeapProps = { };
	HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProps.CreationNodeMask = 0x1;
	HeapProps.VisibleNodeMask = 0x1;

	D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE;

	D3D12_RESOURCE_DESC RtvTd = { };
	RtvTd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	RtvTd.Width = InWidth;
	RtvTd.Height = InHeight;
	RtvTd.MipLevels = 1;
	RtvTd.DepthOrArraySize = 1;
	RtvTd.Format = InFormat;// We can use typeless here (e.g. DXGI_FORMAT_R16_TYPELESS), but we'll use the same format so we don't have to worry about figuring out which typeless format matches. 
	RtvTd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	RtvTd.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL|D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE; // If we wanted a readable depth stencil we wouldn't deny to shader resources.
	RtvTd.SampleDesc.Quality = 0;
	RtvTd.SampleDesc.Count = 1;

	D3D12_CLEAR_VALUE DefaultClearValue = { };
	DefaultClearValue.Format = InFormat;
	DefaultClearValue.DepthStencil.Depth = 1.f;
	DefaultClearValue.DepthStencil.Stencil = 0;

	m_GpuBufferState = D3D12_RESOURCE_STATE_DEPTH_WRITE; // Since we aren't reading from depth stencil, we can start in DEPTH_WRITE and never have to worry about transitions.
	const HRESULT CreateRtRes = InDevice.CreateCommittedResource(&HeapProps, HeapFlags, &RtvTd, m_GpuBufferState, &DefaultClearValue, IID_PPV_ARGS(&m_GpuBuffer));
	if (FAILED(CreateRtRes) || !m_GpuBuffer)
	{
		return;
	}

	// Hold on to a copy of our view provider so we can destroy our view in ~HWD3DBufferBase_DX12
	m_ViewProvider = &InViewProvider;
	m_ViewProvider->AddRef();

	D3D12_DEPTH_STENCIL_VIEW_DESC RtvDesc = { };
	RtvDesc.Format = InFormat;
	RtvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	RtvDesc.Texture2D.MipSlice = 0;
	m_GpuView = InViewProvider.CreateView();
	if (!m_GpuView.IsValid())
	{
		return;
	}

	InDevice.CreateDepthStencilView(m_GpuBuffer, &RtvDesc, m_GpuView.CpuDescHandle);
}