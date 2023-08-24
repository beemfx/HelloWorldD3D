// D3D Hello World

#include "HWD3DViewProvider_DX12.h"

HWD3DViewProvider_DX12* HWD3DViewProvider_DX12::CreateViewProvider(ID3D12Device& InDevice, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, int InMaxDescs)
{
	HWD3DViewProvider_DX12* Out = new HWD3DViewProvider_DX12;
	if (Out)
	{
		Out->Init(InDevice, InHeapType, InMaxDescs);
	}

	return Out;
}

void HWD3DViewProvider_DX12::Init(ID3D12Device& InDevice, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, int InMaxDescs)
{
	m_HeapType = InHeapType;

	const bool bShaderVisible = m_HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	m_Descriptors.resize(InMaxDescs);

	D3D12_DESCRIPTOR_HEAP_DESC Dhd = { };
	Dhd.NumDescriptors = m_Descriptors.size();
	Dhd.Type = m_HeapType;
	Dhd.Flags = bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	const HRESULT Res = InDevice.CreateDescriptorHeap(&Dhd, IID_PPV_ARGS(&m_Heap));
	if (FAILED(Res) || !m_Heap)
	{
		m_Descriptors.resize(0);
		return;
	}

	const UINT IncrementSize = InDevice.GetDescriptorHandleIncrementSize(m_HeapType);

	D3D12_CPU_DESCRIPTOR_HANDLE CpuHeapStart = m_Heap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE GpuHeapStart = { };
	if (bShaderVisible)
	{
		GpuHeapStart = m_Heap->GetGPUDescriptorHandleForHeapStart();
	}

	for (UINT i = 0; i < m_Descriptors.size(); i++)
	{
		hwd3dViewDescriptor& Dst = m_Descriptors[i];
		Dst.Type = m_HeapType;
		Dst.Owner = reinterpret_cast<uintptr_t>(this);
		Dst.InternalId = i + 1;
		Dst.CpuDescHandle.ptr = CpuHeapStart.ptr + i * IncrementSize;
		Dst.GpuDescHandle.ptr = bShaderVisible ? GpuHeapStart.ptr + i * IncrementSize : 0;
	}
}

void HWD3DViewProvider_DX12::Deinit()
{
	// Sanity check:
#if defined(_DEBUG)
	for (auto& Item : m_Descriptors)
	{
		assert(!Item.bInUse);
	}
#endif

	m_Descriptors.resize(0);
	HWD3D_SafeRelease(m_Heap);
}

hwd3dViewDescriptor HWD3DViewProvider_DX12::CreateView()
{
	for (auto& Item : m_Descriptors)
	{
		if (!Item.bInUse)
		{
			Item.bInUse = true;
			return Item;
		}
	}

	// None available, return invalid descriptor.
	return hwd3dViewDescriptor();
}

void HWD3DViewProvider_DX12::DestroyView(const hwd3dViewDescriptor& InDesc)
{
	if (!InDesc.IsValid())
	{
		return;
	}

	if (InDesc.Owner != reinterpret_cast<uintptr_t>(this))
	{
		assert(false); // Destroying View with wrong provider.
		return;
	}

	const size_t DescIdx = InDesc.InternalId - 1;
	if (0 <= DescIdx && DescIdx < m_Descriptors.size())
	{
		assert(m_Descriptors[DescIdx].bInUse && m_Descriptors[DescIdx].InternalId == InDesc.InternalId);
		m_Descriptors[DescIdx].bInUse = false;
	}
}
