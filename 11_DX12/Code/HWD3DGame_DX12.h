// D3D Hello World

#include "HWD3DGame.h"
#include <d3d12.h>
#include <dxgi1_5.h>

class HWD3DGame_DX12 : public HWD3DGame
{
private:
	
	static const int NUM_BACK_BUFFERS = 2;
	
	struct hwd3dDescriptor
	{
		D3D12_CPU_DESCRIPTOR_HANDLE CpuDescHandle = { };
		D3D12_GPU_DESCRIPTOR_HANDLE GpuDescHandle = { };
		D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualPtr = 0;
	};

	struct hwd3dDescriptorHeap
	{
		ID3D12DescriptorHeap* Heap = nullptr;
		std::vector<hwd3dDescriptor> Descriptors;
		int NextDescriptor = 0;

		~hwd3dDescriptorHeap()
		{
			HWD3D_SafeRelease(Heap);
		}

		hwd3dDescriptor GetAvailableDesc()
		{
			if (NextDescriptor < Descriptors.size())
			{
				NextDescriptor++;
				return Descriptors[NextDescriptor-1];
			}

			return hwd3dDescriptor();
		}
	};
	
	struct hwd3dFrameData
	{
		ID3D12Resource* BufferTexture = nullptr;
		ID3D12CommandAllocator* CommandAlloc = nullptr;
		UINT64 FrameFenceValue = 0;

		hwd3dDescriptor Descriptor;

		~hwd3dFrameData()
		{
			HWD3D_SafeRelease(CommandAlloc);
			HWD3D_SafeRelease(BufferTexture);
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

	hwd3dDescriptorHeap m_RenderTargetDescriptors;

	std::vector<hwd3dFrameData> m_FrameData;

	// ID3D12RootSignature* m_RootSig = nullptr;
	ID3D12GraphicsCommandList* m_SwapChainCommandList = nullptr;

	class HWD3DRenderState_DX12* m_Shader = nullptr;

	hwd3d_matrix m_Proj = HWD3DMatrix_Ident;
	hwd3d_matrix m_View = HWD3DMatrix_Ident;
	hwd3d_matrix m_World = HWD3DMatrix_Ident;
	hwd3d_matrix m_ShaderWVP = HWD3DMatrix_Ident;

public:
	
	ID3D12Device* GetDevice() const { return m_D3DDevice; }

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

	bool InitSharedObjects();

	void FlushSwapChain();
	UINT64 SwapChainSignal();
	void SwapChainWaitForFenceValue(UINT64 InValue);

	IDXGIAdapter* PickAdapter();
};
