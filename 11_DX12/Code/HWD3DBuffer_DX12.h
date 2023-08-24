// D3D Hello World

#pragma once

#include "HWD3DObject.h"
#include "HWD3DCore_DX12.h"

enum class hwd3d_buffer_t
{
	None ,
	VertexBuffer ,
	IndexBuffer ,
	ConstantBuffer ,
	Texture ,
};

struct hwd3dTextureFormat
{
	DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
	int Width = 0;
	int Height = 0;
};

class HWD3DBufferBase_DX12 : public HWD3DObject
{
protected:
	
	ID3D12Heap* m_GpuHeap = nullptr;
	ID3D12Resource* m_GpuBuffer = nullptr;
	HWD3DViewProvider_DX12* m_ViewProvider = nullptr;
	DXGI_FORMAT m_GpuBufferFormat = DXGI_FORMAT_UNKNOWN;
	D3D12_RESOURCE_STATES m_GpuBufferState = D3D12_RESOURCE_STATE_COMMON;
	hwd3dViewDescriptor m_GpuView;

public:
	
	void TransitionBuffer(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescHandle() const { return m_GpuView.CpuDescHandle; }

protected:
	
	virtual ~HWD3DBufferBase_DX12() override;
};

class HWD3DBufferRenderTarget_DX12 : public HWD3DBufferBase_DX12
{
private:
	
	

public:
	
	static HWD3DBufferRenderTarget_DX12* CreateRenderTarget(IDXGISwapChain1& InSwapChain, UINT BackBufferIndex, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider);

protected:
	
	void InitRenderTarget(IDXGISwapChain1& InSwapChain, UINT InBackBufferIndex, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider);
};


class HWD3DBufferDepthStencil_DX12 : public HWD3DBufferBase_DX12
{
public:

	static HWD3DBufferDepthStencil_DX12* CreateDepthStencil(DXGI_FORMAT InFormat, int InWidth, int InHeight, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider);

protected:

	void InitDepthStencil(DXGI_FORMAT InFormat, int InWidth, int InHeight, ID3D12Device& InDevice, class HWD3DViewProvider_DX12& InViewProvider);
};

class HWD3DBuffer_DX12 : public HWD3DObject
{
private:

	class HWD3DGame_DX12* m_Game = nullptr;
	ID3D12Heap* m_GpuHeap = nullptr;
	ID3D12Heap* m_UploadHeap = nullptr;
	ID3D12Resource* m_GpuBuffer = nullptr;
	ID3D12Resource* m_UploadBuffer = nullptr;
	hwd3d_buffer_t m_BufferType = hwd3d_buffer_t::None;
	hwd3dTextureFormat m_TextureFormat;
	hwd3dViewDescriptor m_ReadView;

	UINT m_BufferByteSize = 0;
	bool m_bIsValid = false;
	bool m_bBufferNeedsUpload = false;

	D3D12_RESOURCE_STATES m_GpuBufferState = D3D12_RESOURCE_STATE_COMMON;;
	D3D12_RESOURCE_STATES m_UploadBufferState = D3D12_RESOURCE_STATE_COMMON;

public:

	void Init(class HWD3DGame_DX12* InGame, ID3D12Device* InDev, int InSize, hwd3d_buffer_t InBufferType, const hwd3dTextureFormat* InTextureFormat);
	void Deinit();
	bool IsValid() const { return m_bIsValid; }

	void SetBufferData(const void* SourceData, int SourceDataSize);

	UINT64 GetGpuVirtualAddress() const;
	UINT GetBufferByteSize() const { return m_BufferByteSize; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuView() const;
	void PrepareForDraw(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState);

private:

	void TransitionBuffer(ID3D12GraphicsCommandList& Context, D3D12_RESOURCE_STATES TargetState);
};

class HWD3DPerFrameConstantBuffer
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
