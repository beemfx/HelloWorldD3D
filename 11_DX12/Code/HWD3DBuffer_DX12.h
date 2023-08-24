// D3D Hello World

#pragma once

#include "HWD3DTypes.h"
#include <d3d12.h>

class HWD3DBuffer_DX12
{
private:

	ID3D12Heap* m_GpuHeap = nullptr;
	ID3D12Heap* m_UploadHeap = nullptr;
	ID3D12Resource* m_GpuBuffer = nullptr;
	ID3D12Resource* m_UploadBuffer = nullptr;

	UINT m_BufferByteSize = 0;
	bool m_bIsValid = false;
	bool m_bBufferNeedsUpload = false;

	D3D12_RESOURCE_STATES m_GpuBufferState = D3D12_RESOURCE_STATE_COMMON;;
	D3D12_RESOURCE_STATES m_UploadBufferState = D3D12_RESOURCE_STATE_COMMON;

public:

	void Init(ID3D12Device* InDev, int InSize);
	void Deinit();
	bool IsValid() const { return m_bIsValid; }

	void SetBufferData(const void* SourceData, int SourceDataSize);

	UINT64 GetGpuVirtualAddress() const;
	UINT GetBufferByteSize() const { return m_BufferByteSize; }
	void PrepareForDraw(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState);

private:

	void TransitionBuffer(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState);
};

