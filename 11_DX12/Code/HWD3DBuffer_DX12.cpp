// D3D Hello World

#include "HWD3DBuffer_DX12.h"
#include "HWD3DBuffer_DX12.h"
#include "HWD3DGame_DX12.h"

void HWD3DBuffer_DX12::Init(HWD3DGame_DX12* InGame, ID3D12Device* InDev, int InSize, bool bCanRead)
{
	m_Game = InGame;
	assert(!m_GpuBuffer && !m_UploadBuffer && !m_GpuHeap && !m_UploadHeap && !m_bIsValid);
	
	// Align our buffer to the size we want D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT is 256 so we can do some quick math.
	m_BufferByteSize = ((static_cast<UINT>(InSize) + 0xFF - 1) & (~0xFF));
	assert(m_BufferByteSize >= static_cast<UINT>(InSize) && ((m_BufferByteSize%D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT) == 0));

	if (InDev)
	{
		// Gpu Heap
		{
			D3D12_HEAP_DESC HeapProps = {};
			HeapProps.SizeInBytes = m_BufferByteSize;
			HeapProps.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
			HeapProps.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			HeapProps.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			HeapProps.Properties.CreationNodeMask = 0x1;
			HeapProps.Properties.VisibleNodeMask = 0x1;
			HeapProps.Alignment = 0;
			HeapProps.Flags = D3D12_HEAP_FLAG_NONE;

			const HRESULT Res = InDev->CreateHeap(&HeapProps, IID_PPV_ARGS(&m_GpuHeap));

			if (FAILED(Res) || !m_GpuHeap)
			{
				Deinit();
				return;
			}
		}

		// GPU Buffer
		{
			// Fill in a buffer description.

			D3D12_RESOURCE_DESC Desc = {};
			Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			Desc.Alignment = 0;
			Desc.Width = m_BufferByteSize;
			Desc.Height = 1;
			Desc.DepthOrArraySize = 1;
			Desc.MipLevels = 1;
			Desc.Format = DXGI_FORMAT_UNKNOWN;
			Desc.SampleDesc = { 1, 0 };
			Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			m_GpuBufferState = D3D12_RESOURCE_STATE_COMMON;

			const HRESULT nRes = InDev->CreatePlacedResource(m_GpuHeap, 0, &Desc, m_GpuBufferState, nullptr, IID_PPV_ARGS(&m_GpuBuffer));

			if (FAILED(nRes) || !m_GpuBuffer)
			{
				Deinit();
				return;
			}

			if (m_Game && bCanRead)
			{
				m_ReadView = m_Game->GetBufferViewProvider().CreateView();
				m_ReadView.GpuVirtualPtr = m_GpuBuffer->GetGPUVirtualAddress();
				D3D12_CONSTANT_BUFFER_VIEW_DESC Cvd = { };
				Cvd.BufferLocation = m_ReadView.GpuVirtualPtr;
				Cvd.SizeInBytes = m_BufferByteSize;
				InDev->CreateConstantBufferView(&Cvd, m_ReadView.CpuDescHandle);
			}
		}

		// CPU Heap
		{
			D3D12_HEAP_DESC HeapProps = {};
			HeapProps.SizeInBytes = m_BufferByteSize;
			HeapProps.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
			HeapProps.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			HeapProps.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			HeapProps.Properties.CreationNodeMask = 0x1;
			HeapProps.Properties.VisibleNodeMask = 0x1;
			HeapProps.Alignment = 0;
			HeapProps.Flags = D3D12_HEAP_FLAG_NONE;

			const HRESULT Res = InDev->CreateHeap(&HeapProps, IID_PPV_ARGS(&m_UploadHeap));

			if (FAILED(Res) || !m_UploadHeap)
			{
				Deinit();
				return;
			}
		}

		// CPU Buffer
		{
			// Fill in a buffer description.
			D3D12_RESOURCE_DESC Desc = {};
			Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			Desc.Alignment = 0;
			Desc.Width = m_BufferByteSize;
			Desc.Height = 1;
			Desc.DepthOrArraySize = 1;
			Desc.MipLevels = 1;
			Desc.Format = DXGI_FORMAT_UNKNOWN;
			Desc.SampleDesc = { 1, 0 };
			Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			m_UploadBufferState = D3D12_RESOURCE_STATE_GENERIC_READ;

			const HRESULT nRes = InDev->CreatePlacedResource(m_UploadHeap, 0, &Desc, m_UploadBufferState, nullptr, IID_PPV_ARGS(&m_UploadBuffer));

			if (FAILED(nRes) || !m_UploadBuffer)
			{
				Deinit();
				return;
			}
		}
	}

	m_bIsValid = true;
}

void HWD3DBuffer_DX12::Deinit()
{
	HWD3D_SafeRelease(m_UploadBuffer);
	HWD3D_SafeRelease(m_UploadHeap);
	HWD3D_SafeRelease(m_GpuBuffer);
	HWD3D_SafeRelease(m_GpuHeap);

	m_UploadBufferState = D3D12_RESOURCE_STATE_COMMON;
	m_GpuBufferState = D3D12_RESOURCE_STATE_COMMON;

	if (m_ReadView.IsValid() && m_Game)
	{
		m_Game->GetBufferViewProvider().DestroyView(m_ReadView);
	}

	m_Game = nullptr;

	m_bIsValid = false;
}

void HWD3DBuffer_DX12::SetBufferData(const void* SourceData, int SourceDataSize)
{
	assert(static_cast<UINT>(SourceDataSize) <= m_BufferByteSize);

	void* pMappedData = nullptr;
	if (SUCCEEDED(m_UploadBuffer->Map(0, nullptr, &pMappedData)))
	{
		memcpy(pMappedData, SourceData, SourceDataSize);
		m_UploadBuffer->Unmap(0, nullptr);
	}

	m_bBufferNeedsUpload = true;
}

UINT64 HWD3DBuffer_DX12::GetReadViewAddress() const
{
	return m_ReadView.GpuVirtualPtr;
}

UINT64 HWD3DBuffer_DX12::GetGpuVirtualAddress() const
{
	return m_GpuBuffer ? m_GpuBuffer->GetGPUVirtualAddress() : 0;
}

void HWD3DBuffer_DX12::PrepareForDraw(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState)
{
	if (m_bBufferNeedsUpload)
	{
		m_bBufferNeedsUpload = false;

		TransitionBuffer(Context, D3D12_RESOURCE_STATE_COPY_DEST);
		Context.CopyBufferRegion(m_GpuBuffer, 0, m_UploadBuffer, 0, m_BufferByteSize);
	}

	TransitionBuffer(Context, TargetState);
}

void HWD3DBuffer_DX12::TransitionBuffer(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState)
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

void HWD3DPerFrameBuffer::Init(class HWD3DGame_DX12* InGame, int InDataSize)
{
	m_Game = InGame;
	m_DataSize = InDataSize;
}

void HWD3DPerFrameBuffer::Deinit()
{
	for (auto* Item : m_Buffers)
	{
		Item->Deinit();
		delete Item;
	}
}

void HWD3DPerFrameBuffer::BeginFrame()
{
	m_NextBuffer = 0;
}

void HWD3DPerFrameBuffer::SetData(ID3D12GraphicsCommandList& Context, const void* SourceData, int SourceDataSize)
{
	assert(SourceDataSize <= m_DataSize);

	if (0 <= m_NextBuffer && m_NextBuffer < static_cast<int>(m_Buffers.size()))
	{
		HWD3DBuffer_DX12* Buffer = m_Buffers[m_NextBuffer];
		m_NextBuffer++;
		Buffer->SetBufferData(SourceData, SourceDataSize);
		Context.SetGraphicsRootConstantBufferView(0, Buffer->GetReadViewAddress());
		Buffer->PrepareForDraw(Context,  D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}
	else
	{
		// Need a new buffer, so create one:
		HWD3DBuffer_DX12* NewBuffer = new HWD3DBuffer_DX12;
		if (NewBuffer)
		{
			m_Buffers.push_back(NewBuffer);
			m_NextBuffer = m_Buffers.size();
			NewBuffer->Init(m_Game, m_Game->GetDevice(), m_DataSize, true);
			NewBuffer->SetBufferData(SourceData, SourceDataSize);
			Context.SetGraphicsRootConstantBufferView(0, NewBuffer->GetReadViewAddress());
			NewBuffer->PrepareForDraw(Context,  D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}
	}
}
