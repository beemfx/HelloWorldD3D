// D3D Hello World

#include "HWD3DFrameData_DX12.h"
#include "HWD3DBufferRenderTarget_DX12.h"
#include "HWD3DBufferConstant_DX12.h"
#include "HWD3DGame_DX12.h"

bool HWD3DFrameData_DX12::Init(class HWD3DGame_DX12* InOwner, IDXGISwapChain4& InSwapChain, UINT InBbIndex, ID3D12Device& InDev, class HWD3DViewProvider_DX12& InViewProvider, UINT InConstantBufferSize)
{
	m_RenderTarget = HWD3DBufferRenderTarget_DX12::CreateRenderTarget(InSwapChain, InBbIndex, InDev, InViewProvider);
	if (!m_RenderTarget)
	{
		return false;
	}

	m_CBMgr.Init(InOwner, InConstantBufferSize);

	// Every back buffer needs an allocator.
	const HRESULT CcaRes = InDev.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAlloc));
	if (FAILED(CcaRes) && !m_CommandAlloc)
	{
		return false;
	}

	return true;
}

void HWD3DFrameData_DX12::Deinit()
{
	m_CBMgr.Deinit();
	HWD3D_SafeRelease(m_CommandAlloc);
	HWD3D_SafeRelease(m_RenderTarget);
}

D3D12_CPU_DESCRIPTOR_HANDLE HWD3DFrameData_DX12::GetRenderTargetCpuDescHandle() const
{
	return m_RenderTarget ? m_RenderTarget->GetCpuDescHandle() : D3D12_CPU_DESCRIPTOR_HANDLE { 0 };
}

void HWD3DFrameData_DX12::BeginFrame()
{
	m_CBMgr.BeginFrame();
}

void HWD3DFrameData_DX12::PrepareToDraw(ID3D12GraphicsCommandList& CmdList)
{
	if (m_RenderTarget)
	{
		m_RenderTarget->TransitionBuffer(CmdList, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
}

void HWD3DFrameData_DX12::PrepareToPresent(ID3D12GraphicsCommandList& CmdList)
{
	if (m_RenderTarget)
	{
		m_RenderTarget->TransitionBuffer(CmdList, D3D12_RESOURCE_STATE_PRESENT);
	}
}

void HWD3DFrameData_DX12::UpdateConstantBuffer(ID3D12GraphicsCommandList& CmdList, const void* BufferData, UINT DataSize)
{
	m_CBMgr.SetData(CmdList, BufferData, DataSize);
}

void HWD3DPerFrameConstantBufferManager_DX12::Init(class HWD3DGame_DX12* InGame, int InDataSize)
{
	m_Game = InGame;
	m_DataSize = InDataSize;
}

void HWD3DPerFrameConstantBufferManager_DX12::Deinit()
{
	for (auto* Item : m_Buffers)
	{
		HWD3D_SafeRelease(Item);
	}
	m_Buffers.resize(0);
}

void HWD3DPerFrameConstantBufferManager_DX12::BeginFrame()
{
	m_NextBuffer = 0;
}

void HWD3DPerFrameConstantBufferManager_DX12::SetData(ID3D12GraphicsCommandList& Context, const void* SourceData, int SourceDataSize)
{
	assert(SourceDataSize <= m_DataSize);

	if (0 <= m_NextBuffer && m_NextBuffer < static_cast<int>(m_Buffers.size()))
	{
		HWD3DBufferConstant_DX12* Buffer = m_Buffers[m_NextBuffer];
		m_NextBuffer++;
		Buffer->SetBufferData(SourceData, SourceDataSize);
		Context.SetGraphicsRootConstantBufferView(0, Buffer->GetGpuVirtualAddress());
		Buffer->PrepareForDraw(Context);
	}
	else
	{
		// Need a new buffer, so create one:
		HWD3DBufferConstant_DX12* NewBuffer = HWD3DBufferConstant_DX12::CreateConstantBuffer(hwd3d_constant_buffer_t::ConstantBuffer, m_DataSize, *m_Game->GetDevice());;
		if (NewBuffer)
		{
			m_Buffers.push_back(NewBuffer);
			m_NextBuffer = m_Buffers.size();
			NewBuffer->SetBufferData(SourceData, SourceDataSize);
			Context.SetGraphicsRootConstantBufferView(0, NewBuffer->GetGpuVirtualAddress());
			NewBuffer->PrepareForDraw(Context);
		}
	}
}