// D3D Hello World

#pragma once

#include "HWD3DObject.h"
#include "HWD3DCore_DX12.h"

class HWD3DViewProvider_DX12 : public HWD3DObject
{
public:
	
	static HWD3DViewProvider_DX12* CreateViewProvider(ID3D12Device& InDevice, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, int InMaxDescs);

private:
	
	ID3D12DescriptorHeap* m_Heap = nullptr;
	std::vector<hwd3dViewDescriptor> m_Descriptors;
	D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

protected:
	
	virtual void OnObjectDestruct() override { Deinit(); }
	void Init(ID3D12Device& InDevice, D3D12_DESCRIPTOR_HEAP_TYPE InHeapType, int InMaxDescs);
	void Deinit();

public:

	ID3D12DescriptorHeap* GetHeap() const { return m_Heap; }
	
	hwd3dViewDescriptor CreateView();
	void DestroyView(const hwd3dViewDescriptor& InDesc);
};
