// D3D Hello World

#pragma once

#include "HWD3DObject.h"
#include "HWD3DCore_DX12.h"

class HWD3DBufferBase_DX12 : public HWD3DObject
{
protected:
	
	UINT m_BufferByteSize = 0;
	ID3D12Heap* m_GpuHeap = nullptr;
	ID3D12Resource* m_GpuBuffer = nullptr;
	D3D12_RESOURCE_STATES m_GpuBufferState = D3D12_RESOURCE_STATE_COMMON;
	DXGI_FORMAT m_GpuBufferFormat = DXGI_FORMAT_UNKNOWN;
	hwd3dViewDescriptor m_GpuView;
	ID3D12Heap* m_UploadHeap = nullptr;
	ID3D12Resource* m_UploadBuffer = nullptr;
	D3D12_RESOURCE_STATES m_UploadBufferState = D3D12_RESOURCE_STATE_COMMON;
	HWD3DViewProvider_DX12* m_ViewProvider = nullptr;
	bool m_bBufferNeedsUpload = false;

public:
	
	void TransitionBuffer(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescHandle() const { return m_GpuView.CpuDescHandle; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescHandle() const { return m_GpuView.GpuDescHandle; }
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return m_GpuView.GpuVirtualPtr; }
	UINT GetBufferSize() const { return m_BufferByteSize; }
	void SetBufferData(const void* SourceData, int SourceDataSize);
	bool IsValid() const { return m_GpuBuffer != nullptr; }

protected:
	
	virtual ~HWD3DBufferBase_DX12() override;
};
