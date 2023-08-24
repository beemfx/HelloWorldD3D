// D3D Hello World

#pragma once

#include "HWD3DTypes.h"
#include <d3d12.h>

struct hwd3dViewDescriptor
{
	D3D12_CPU_DESCRIPTOR_HANDLE CpuDescHandle = { };
	D3D12_GPU_DESCRIPTOR_HANDLE GpuDescHandle = { };
	D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualPtr = 0;
	D3D12_DESCRIPTOR_HEAP_TYPE Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	uintptr_t Owner = 0;
	int InternalId = 0;
	bool bInUse = false;
	UINT Size = 0;

	bool IsValid() const { return InternalId != 0; }
	void Invalidate() { InternalId = 0; }
};

class HWD3DViewProvider_DX12
{
private:
	
	ID3D12DescriptorHeap* m_Heap = nullptr;
	std::vector<hwd3dViewDescriptor> m_Descriptors;
	D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

public:
	
	void Init(ID3D12Device& InDevice, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, int InMaxDescs);
	void Deinit();

	ID3D12DescriptorHeap* GetHeap() const { return m_Heap; }
	
	hwd3dViewDescriptor CreateView();
	void DestroyView(const hwd3dViewDescriptor& InDesc);
};
