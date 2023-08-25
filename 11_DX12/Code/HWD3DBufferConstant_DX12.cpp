// D3D Hello World

#include "HWD3DBufferConstant_DX12.h"
#include "HWD3DGame_DX12.h"

HWD3DBufferConstant_DX12* HWD3DBufferConstant_DX12::CreateConstantBuffer(hwd3d_constant_buffer_t InType, UINT InSize, ID3D12Device& InDev)
{
	HWD3DBufferConstant_DX12* Out = new HWD3DBufferConstant_DX12;
	if (Out)
	{
		Out->Init(InType, InSize, InDev);
	}
	return Out;
}


void HWD3DBufferConstant_DX12::Init(hwd3d_constant_buffer_t InType, UINT InSize, ID3D12Device& InDev)
{
	m_ConstantBufferType = InType;

	// Align our buffer to the size we want D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT is 256 so we can do some quick math.
	m_BufferByteSize = ((static_cast<UINT>(InSize) + 0xFF - 1) & (~0xFF));
	assert(m_BufferByteSize >= static_cast<UINT>(InSize) && ((m_BufferByteSize%D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT) == 0));

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

		const HRESULT Res = InDev.CreateHeap(&HeapProps, IID_PPV_ARGS(&m_GpuHeap));

		if (FAILED(Res) || !m_GpuHeap)
		{
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

		const HRESULT nRes = InDev.CreatePlacedResource(m_GpuHeap, 0, &Desc, m_GpuBufferState, nullptr, IID_PPV_ARGS(&m_GpuBuffer));

		if (FAILED(nRes) || !m_GpuBuffer)
		{
			return;
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

		const HRESULT Res = InDev.CreateHeap(&HeapProps, IID_PPV_ARGS(&m_UploadHeap));

		if (FAILED(Res) || !m_UploadHeap)
		{
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

		const HRESULT nRes = InDev.CreatePlacedResource(m_UploadHeap, 0, &Desc, m_UploadBufferState, nullptr, IID_PPV_ARGS(&m_UploadBuffer));

		if (FAILED(nRes) || !m_UploadBuffer)
		{
			return;
		}
	}


	m_GpuView.GpuVirtualPtr = m_GpuBuffer->GetGPUVirtualAddress();
}

void HWD3DBufferConstant_DX12::PrepareForDraw(ID3D12GraphicsCommandList& Context)
{
	if (m_bBufferNeedsUpload)
	{
		m_bBufferNeedsUpload = false;

		TransitionBuffer(Context, D3D12_RESOURCE_STATE_COPY_DEST);
		Context.CopyBufferRegion(m_GpuBuffer, 0, m_UploadBuffer, 0, m_BufferByteSize);
	}


	D3D12_RESOURCE_STATES TargetState = D3D12_RESOURCE_STATE_COMMON;
	switch (m_ConstantBufferType)
	{
	case hwd3d_constant_buffer_t::None:
		break;
	case hwd3d_constant_buffer_t::VertexBuffer:
		TargetState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		break;
	case hwd3d_constant_buffer_t::IndexBuffer:
		TargetState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
		break;
	case hwd3d_constant_buffer_t::ConstantBuffer:
		TargetState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		break;
	}

	TransitionBuffer(Context, TargetState);
}
