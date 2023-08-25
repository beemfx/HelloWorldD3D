// D3D Hello World

#pragma once

#include "HWD3DGame.h"
#include "HWD3DCore_DX12.h"

class HWD3DGame_DX12 : public HWD3DGame
{
private:
	
	class HWD3DFrameData;
	
	static const int NUM_BACK_BUFFERS = 2;
	static const int MAX_TEXTURE_VIEWS = 128;  // Our little demo doesn't really use this many textures but we may as well have enough.

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

	class HWD3DViewProvider_DX12* m_RenderTargetViewProvider;
	class HWD3DViewProvider_DX12* m_DepthStencilViewProvider;
	class HWD3DViewProvider_DX12* m_TextureBufferViewProvider;

	std::vector<HWD3DFrameData> m_FrameData;
	UINT m_CurrentFrameDataIndex = 0xFFFFFFFF;
	HWD3DFrameData* m_CurrentFrameData = nullptr;

	class HWD3DBufferDepthStencil_DX12* m_DepthStencil = nullptr;

	ID3D12RootSignature* m_RootSig = nullptr;
	ID3D12GraphicsCommandList* m_SwapChainCommandList = nullptr;

	class HWD3DRenderState_DX12* m_Shader = nullptr;

	hwd3d_matrix m_Proj = HWD3DMatrix_Ident;
	hwd3d_matrix m_View = HWD3DMatrix_Ident;
	hwd3d_matrix m_World = HWD3DMatrix_Ident;
	hwd3d_matrix m_ShaderWVP = HWD3DMatrix_Ident;
	bool m_bShouldClearAtStartOfNextFrame = false;

public:
	
	ID3D12Device* GetDevice() const { return m_D3DDevice; }
	ID3D12RootSignature* GetRootSig() const { return m_RootSig; }
	ID3D12GraphicsCommandList* GetCommandList() const { return m_SwapChainCommandList; }
	HWD3DViewProvider_DX12& GetTextureBufferViewProvider() { return *m_TextureBufferViewProvider; }

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

private:
	
	class HWD3DFrameData
	{
	private:

		class HWD3DDrawBuffers
		{
		private:

			ID3D12Device* m_Device = nullptr;
			int m_DataSize = 0;
			std::vector<class HWD3DBufferConstant_DX12*> m_PerUpdateBuffers;
			int m_NextBuffer = 0;

		public:

			void Init(ID3D12Device* InDevice, int InSize);
			void Deinit();
			void BeginFrame();
			void SetData(ID3D12GraphicsCommandList& Context, const void* SourceData, int SourceDataSize);
		};

	private:

		class HWD3DBufferRenderTarget_DX12* m_RenderTarget = nullptr;
		ID3D12CommandAllocator* m_CommandAlloc = nullptr;
		UINT64 m_FrameFenceValue = 0;
		HWD3DDrawBuffers m_CBMgr;

	public:

		~HWD3DFrameData()
		{
			assert(!m_CommandAlloc && !m_RenderTarget); // Should have called Deinit before destroying.
		}

		bool Init(class HWD3DGame_DX12& InOwner, IDXGISwapChain4& InSwapChain, UINT InBbIndex, ID3D12Device& InDev, class HWD3DViewProvider_DX12& InViewProvider, UINT InConstantBufferSize);
		void Deinit();

		UINT64 GetFrameFencValue() const { return m_FrameFenceValue; }
		void SetFrameFenceValue(UINT64 InNewValue) { m_FrameFenceValue = InNewValue; }
		ID3D12CommandAllocator* GetCommandAlloc() const { return m_CommandAlloc; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetCpuDescHandle() const;

		void PrepareToDraw(ID3D12GraphicsCommandList& CmdList);
		void PrepareToPresent(ID3D12GraphicsCommandList& CmdList);
		void UpdateConstantBuffer(ID3D12GraphicsCommandList& CmdList, const void* BufferData, UINT DataSize);
	};
};
