// D3D Hello World

#include "HWD3DGame.h"
#include "HWD3DViewProvider_DX12.h"
#include "HWD3DBuffer_DX12.h"
#include <d3d12.h>
#include <dxgi1_5.h>

class HWD3DGame_DX12 : public HWD3DGame
{
private:
	
	static const int NUM_BACK_BUFFERS = 2;
	static const int NUM_RESOURCE_VIEWS = 128;  // Our little demo doesn't really use this many views but we may as well have enough.
	
	struct hwd3dFrameData
	{
		ID3D12Resource* BufferTexture = nullptr;
		ID3D12CommandAllocator* CommandAlloc = nullptr;
		UINT64 FrameFenceValue = 0;

		HWD3DPerFrameConstantBuffer ConstantBuffer;

		hwd3dViewDescriptor BufferDescriptor;
		D3D12_RESOURCE_STATES BufferState = D3D12_RESOURCE_STATE_COMMON;

		void PrepareToDraw(ID3D12GraphicsCommandList& CmdList)
		{
			if( BufferState != D3D12_RESOURCE_STATE_RENDER_TARGET )
			{
				D3D12_RESOURCE_BARRIER Barrier = { };
				Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				Barrier.Transition.pResource = BufferTexture;
				Barrier.Transition.StateBefore = BufferState;
				Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
				BufferState = D3D12_RESOURCE_STATE_RENDER_TARGET;
				CmdList.ResourceBarrier( 1 , &Barrier );
			}
		}

		void PrepareToPresent(ID3D12GraphicsCommandList& CmdList)
		{
			if( BufferState != D3D12_RESOURCE_STATE_PRESENT )
			{
				D3D12_RESOURCE_BARRIER Barrier = { };
				Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				Barrier.Transition.pResource = BufferTexture;
				Barrier.Transition.StateBefore = BufferState;
				Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
				BufferState = D3D12_RESOURCE_STATE_PRESENT;
				CmdList.ResourceBarrier( 1 , &Barrier );
			}
		}

		void Deinit(HWD3DViewProvider_DX12& InVewProvider)
		{
			InVewProvider.DestroyView(BufferDescriptor);
			BufferDescriptor.Invalidate();
			ConstantBuffer.Deinit();
			HWD3D_SafeRelease(CommandAlloc);
			HWD3D_SafeRelease(BufferTexture);
		}

		~hwd3dFrameData()
		{
			assert(!CommandAlloc && !BufferTexture && !BufferDescriptor.IsValid()); // Should have called Deinit before destroying.
		}
	};

private:
	
	HWND m_TargetWnd = nullptr;
	UINT m_ViewWidth = 0;
	UINT m_ViewHeight = 0;

	IDXGIFactory4* m_GiFactory = nullptr;
	IDXGIAdapter* m_GiAdapter = nullptr;
	ID3D12Device* m_D3DDevice = nullptr;
	ID3D12CommandQueue* m_CommandQueue = nullptr;
	IDXGISwapChain4* m_SwapChain = nullptr;
	ID3D12Fence* m_SwapChainFence = nullptr;
	UINT64 m_SwapChainFenceValue = 0;
	HANDLE m_SwapChainFenceEventHandle = 0;

	HWD3DViewProvider_DX12 m_RenderTargetViewProvider;
	HWD3DViewProvider_DX12 m_DepthStencilViewProvider;
	HWD3DViewProvider_DX12 m_BufferViewProvider;

	std::vector<hwd3dFrameData> m_FrameData;
	UINT m_CurrentFrameDataIndex = 0xFFFFFFFF;
	hwd3dFrameData* m_CurrentFrameData = nullptr;

	ID3D12Resource* m_DepthStencilTexture = nullptr;
	hwd3dViewDescriptor m_DepthStencilView;
	D3D12_RESOURCE_STATES m_DepthStencilState = D3D12_RESOURCE_STATE_COMMON;

	ID3D12RootSignature* m_RootSig = nullptr;
	ID3D12GraphicsCommandList* m_SwapChainCommandList = nullptr;

	class HWD3DRenderState_DX12* m_Shader = nullptr;

	hwd3d_matrix m_Proj = HWD3DMatrix_Ident;
	hwd3d_matrix m_View = HWD3DMatrix_Ident;
	hwd3d_matrix m_World = HWD3DMatrix_Ident;
	hwd3d_matrix m_ShaderWVP = HWD3DMatrix_Ident;
	bool m_bConstantBufferDirty = false;

	bool m_bShouldClearAtStartOfNextFrame = false;

public:
	
	ID3D12Device* GetDevice() const { return m_D3DDevice; }
	ID3D12RootSignature* GetRootSig() const { return m_RootSig; }
	ID3D12GraphicsCommandList* GetCommandList() const { return m_SwapChainCommandList; }
	HWD3DViewProvider_DX12& GetBufferViewProvider() { return m_BufferViewProvider; }

	void PreDraw();

private:

	virtual void InitDevice( HWND TargetWnd) override;
	virtual void DeinitDevice() override;

	virtual void ClearViewport() override;
	virtual bool BeginDraw() override;
	virtual void EndDraw() override;
	virtual void Present() override;

	virtual void SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix) override;

	bool InitDescriptors();
	bool InitBackBuffer();
	bool InitDepthStencil();

	bool InitSharedObjects();

	void FlushSwapChain();
	UINT64 SwapChainSignal();
	void SwapChainWaitForFenceValue(UINT64 InValue);

	IDXGIAdapter* PickAdapter();
};