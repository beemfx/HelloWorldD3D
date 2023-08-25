// D3D Hello World

#include "HWD3DBufferTexture_DX12.h"
#include "HWD3DViewProvider_DX12.h"

HWD3DBufferTexture_DX12* HWD3DBufferTexture_DX12::CreateTexture(ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider, const hwd3dTextureFormat& InTextureFormat)
{
	HWD3DBufferTexture_DX12* Out = new HWD3DBufferTexture_DX12;
	if (Out)
	{
		Out->InitTexture(InDevice, InViewProvider, InTextureFormat);
	}

	return Out;
}

void HWD3DBufferTexture_DX12::PrepareForDraw(ID3D12GraphicsCommandList& Context)
{
	if (m_bBufferNeedsUpload)
	{
		m_bBufferNeedsUpload = false;

		TransitionBuffer(Context, D3D12_RESOURCE_STATE_COPY_DEST);
		D3D12_TEXTURE_COPY_LOCATION CopyDst = { };
		CopyDst.pResource = m_GpuBuffer;
		CopyDst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		CopyDst.SubresourceIndex = 0;
		D3D12_TEXTURE_COPY_LOCATION CopySrc = { };
		CopySrc.pResource = m_UploadBuffer;
		CopySrc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

		CopySrc.PlacedFootprint.Offset = 0;
		CopySrc.PlacedFootprint.Footprint.Format = m_TextureFormat.Format;
		CopySrc.PlacedFootprint.Footprint.Width = m_TextureFormat.Width;
		CopySrc.PlacedFootprint.Footprint.Height = m_TextureFormat.Height;
		CopySrc.PlacedFootprint.Footprint.RowPitch = m_TextureFormat.Width * 4;
		CopySrc.PlacedFootprint.Footprint.Depth = 1;

		// D3D12_BOX CopyBox = { 0 , 0 , 0 , m_TextureFormat.Width , m_TextureFormat.Height , 1 };
		Context.CopyTextureRegion(&CopyDst, 0, 0, 0, &CopySrc, NULL);//&CopyBox);
	}

	TransitionBuffer(Context, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void HWD3DBufferTexture_DX12::InitTexture(ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider, const hwd3dTextureFormat& InTextureFormat)
{
	m_TextureFormat = InTextureFormat;

	assert(m_TextureFormat.Format == DXGI_FORMAT_R8G8B8A8_UNORM);
	m_BufferByteSize = m_TextureFormat.Width * m_TextureFormat.Height * 4; // Assuming R8G8B8A8_UNORM;

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

		const HRESULT Res = InDevice.CreateHeap(&HeapProps, IID_PPV_ARGS(&m_GpuHeap));

		if (FAILED(Res) || !m_GpuHeap)
		{
			return;
		}
	}

	// GPU Buffer
	{
		// Fill in a buffer description.
		D3D12_RESOURCE_DESC Desc = {};
		Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		Desc.Alignment = 0;
		Desc.Width = m_TextureFormat.Width;
		Desc.Height = m_TextureFormat.Height;
		Desc.DepthOrArraySize = 1;
		Desc.MipLevels = 1;
		Desc.Format = m_TextureFormat.Format;
		Desc.SampleDesc = { 1, 0 };
		Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		UINT64 RequiredSize = 0;

		InDevice.GetCopyableFootprints(&Desc, 0, 1, 0, nullptr, nullptr, nullptr, &RequiredSize);
		RequiredSize = RequiredSize;
		assert(RequiredSize == m_BufferByteSize); // Created heap of wrong size, we need to rethink this.
		

		m_GpuBufferState = D3D12_RESOURCE_STATE_COMMON;

		const HRESULT nRes =InDevice.CreatePlacedResource(m_GpuHeap, 0, &Desc, m_GpuBufferState, nullptr, IID_PPV_ARGS(&m_GpuBuffer));

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

		const HRESULT Res = InDevice.CreateHeap(&HeapProps, IID_PPV_ARGS(&m_UploadHeap));

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

		const HRESULT nRes = InDevice.CreatePlacedResource(m_UploadHeap, 0, &Desc, m_UploadBufferState, nullptr, IID_PPV_ARGS(&m_UploadBuffer));

		if (FAILED(nRes) || !m_UploadBuffer)
		{

			return;
		}
	}

	m_ViewProvider = &InViewProvider;
	m_ViewProvider->AddRef();

	m_GpuView = InViewProvider.CreateView();
	D3D12_SHADER_RESOURCE_VIEW_DESC Svd = { };
	Svd.Format = m_TextureFormat.Format;
	Svd.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	Svd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	Svd.Texture2D.MostDetailedMip = 0;
	Svd.Texture2D.MipLevels = 1;
	Svd.Texture2D.PlaneSlice = 0;
	Svd.Texture2D.ResourceMinLODClamp = 0.f;

	InDevice.CreateShaderResourceView(m_GpuBuffer, &Svd, m_GpuView.CpuDescHandle);
}
