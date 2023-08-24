// D3D Hello World

#pragma once

#include "HWD3DTypes.h"
#include <d3d12.h>
#include <dxgi1_5.h>

struct hwd3dViewDescriptor
{
friend class HWD3DViewProvider_DX12;

public:
	
	D3D12_CPU_DESCRIPTOR_HANDLE CpuDescHandle = { };
	D3D12_GPU_DESCRIPTOR_HANDLE GpuDescHandle = { };
	D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualPtr = 0;
	D3D12_DESCRIPTOR_HEAP_TYPE Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	uintptr_t Owner = 0;
	int InternalId = 0;
	bool bInUse = false;
	UINT Size = 0;

public:
	
	bool IsValid() const { return InternalId != 0; }
	void Invalidate() { InternalId = 0; }
};
