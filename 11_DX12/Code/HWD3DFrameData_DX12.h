// D3D Hello World

#pragma once

#include "HWD3DCore_DX12.h"

class HWD3DPerFrameConstantBuffer
{
private:

	class HWD3DGame_DX12* m_Game = nullptr;
	int m_DataSize = 0;
	std::vector<class HWD3DBufferConstant_DX12*> m_Buffers;
	int m_NextBuffer = 0;

public:

	void Init(class HWD3DGame_DX12* InGame, int InSize);
	void Deinit();
	void BeginFrame();
	void SetData(ID3D12GraphicsCommandList& Context, const void* SourceData, int SourceDataSize);
};


class HWD3DFrameData_DX12
{
private:
	
	class HWD3DBufferRenderTarget_DX12* m_RenderTarget = nullptr;
	ID3D12CommandAllocator* m_CommandAlloc = nullptr;
	UINT64 m_FrameFenceValue = 0;
	HWD3DPerFrameConstantBuffer m_ConstantBuffer;

public:
	
	~HWD3DFrameData_DX12()
	{
		assert(!m_CommandAlloc && !m_RenderTarget); // Should have called Deinit before destroying.
	}

	bool Init(class HWD3DGame_DX12* InOwner, IDXGISwapChain4& InSwapChain, UINT InBbIndex, ID3D12Device& InDev, class HWD3DViewProvider_DX12& InViewProvider, UINT InConstantBufferSize);
	void Deinit();

	UINT64 GetFrameFencValue() const { return m_FrameFenceValue; }
	void SetFrameFenceValue(UINT64 InNewValue) { m_FrameFenceValue = InNewValue; }
	ID3D12CommandAllocator* GetCommandAlloc() const { return m_CommandAlloc; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetCpuDescHandle() const;

	void BeginFrame();
	void PrepareToDraw(ID3D12GraphicsCommandList& CmdList);
	void PrepareToPresent(ID3D12GraphicsCommandList& CmdList);
	void UpdateConstantBuffer(ID3D12GraphicsCommandList& CmdList, const void* BufferData, UINT DataSize);
};
