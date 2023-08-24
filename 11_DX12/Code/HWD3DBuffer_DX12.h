// D3D Hello World

#pragma once

#include "HWD3DTypes.h"
#include "HWD3DViewProvider_DX12.h"
#include <d3d12.h>

class HWD3DBuffer_DX12
{
private:

	class HWD3DGame_DX12* m_Game = nullptr;
	ID3D12Heap* m_GpuHeap = nullptr;
	ID3D12Heap* m_UploadHeap = nullptr;
	ID3D12Resource* m_GpuBuffer = nullptr;
	ID3D12Resource* m_UploadBuffer = nullptr;
	hwd3dViewDescriptor m_ReadView;

	UINT m_BufferByteSize = 0;
	bool m_bIsValid = false;
	bool m_bBufferNeedsUpload = false;

	D3D12_RESOURCE_STATES m_GpuBufferState = D3D12_RESOURCE_STATE_COMMON;;
	D3D12_RESOURCE_STATES m_UploadBufferState = D3D12_RESOURCE_STATE_COMMON;

public:

	void Init(class HWD3DGame_DX12* InGame, ID3D12Device* InDev, int InSize, bool bCanRead);
	void Deinit();
	bool IsValid() const { return m_bIsValid; }

	void SetBufferData(const void* SourceData, int SourceDataSize);

	UINT64 GetReadViewAddress() const;
	UINT64 GetGpuVirtualAddress() const;
	UINT GetBufferByteSize() const { return m_BufferByteSize; }
	void PrepareForDraw(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState);

private:

	void TransitionBuffer(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState);
};

class HWD3DPerFrameBuffer
{
private:
	
	class HWD3DGame_DX12* m_Game = nullptr;
	int m_DataSize = 0;
	std::vector<HWD3DBuffer_DX12*> m_Buffers;
	int m_NextBuffer = 0;

public:
	
	void Init(class HWD3DGame_DX12* InGame, int InSize);
	void Deinit();
	void BeginFrame();
	void SetData(ID3D12GraphicsCommandList& Context, const void* SourceData, int SourceDataSize);
};
