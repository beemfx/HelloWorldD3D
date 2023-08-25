// D3D Hello World

#include "HWD3DBuffer_DX12.h"
#include "HWD3DGame_DX12.h"
#include "HWD3DViewProvider_DX12.h"

void HWD3DBufferBase_DX12::TransitionBuffer(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState)
{
	if (m_GpuBufferState != TargetState)
	{
		D3D12_RESOURCE_BARRIER Barrier = { };
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = m_GpuBuffer;
		Barrier.Transition.StateBefore = m_GpuBufferState;
		Barrier.Transition.StateAfter = TargetState;
		m_GpuBufferState = TargetState;
		Context.ResourceBarrier(1, &Barrier);
	}
}

void HWD3DBufferBase_DX12::SetBufferData(const void* SourceData, int SourceDataSize)
{
	assert(m_UploadBuffer); // This type of buffer doesn't accept setting data
	assert(static_cast<UINT>(SourceDataSize) <= m_BufferByteSize);

	void* pMappedData = nullptr;
	if (SUCCEEDED(m_UploadBuffer->Map(0, nullptr, &pMappedData)))
	{
		memcpy(pMappedData, SourceData, SourceDataSize);
		m_UploadBuffer->Unmap(0, nullptr);
	}

	m_bBufferNeedsUpload = true;
}

HWD3DBufferBase_DX12::~HWD3DBufferBase_DX12()
{
	HWD3D_SafeRelease(m_GpuBuffer);
	HWD3D_SafeRelease(m_GpuHeap);
	if (m_ViewProvider && m_GpuView.IsValid())
	{
		m_ViewProvider->DestroyView(m_GpuView);
	}
	HWD3D_SafeRelease(m_ViewProvider);

	HWD3D_SafeRelease(m_UploadBuffer);
	HWD3D_SafeRelease(m_UploadHeap);
}
