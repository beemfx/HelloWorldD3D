// D3D Hello World

#include "HWD3DGame_DX12.h"
#include "HWD3DRenderState_DX12.h"

extern "C" { _declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001; }
extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	SetWindowTextW(InMainWnd, L"Hello World D3D [DX12]");

	HWD3DGame* Out = new HWD3DGame_DX12;
	if (Out)
	{
		Out->Init(InMainWnd);
	}

	return Out;
}

void HWD3DGame_DX12::PreDraw()
{
	assert(m_CurrentFrameData && m_SwapChainCommandList); // This should only happen while building command list.
	if (m_CurrentFrameData && m_SwapChainCommandList)
	{
		if (m_bConstantBufferDirty)
		{
			m_bConstantBufferDirty = false;

			m_CurrentFrameData->ConstantBuffer.SetData(*m_SwapChainCommandList, &m_ShaderWVP, sizeof(m_ShaderWVP));
		}
	}
}

void HWD3DGame_DX12::InitDevice(HWND TargetWnd)
{
	m_TargetWnd = TargetWnd;

	RECT TargetWndRc = {};
	GetClientRect(m_TargetWnd, &TargetWndRc);
	m_ViewWidth = TargetWndRc.right - TargetWndRc.left;
	m_ViewHeight = TargetWndRc.bottom - TargetWndRc.top;

	// Init:
	{
#if defined(_DEBUG)
		ID3D12Debug* D3DDebug = nullptr;
		const HRESULT GetDebugRes = D3D12GetDebugInterface(IID_PPV_ARGS(&D3DDebug));
		if (SUCCEEDED(GetDebugRes) && D3DDebug)
		{
			D3DDebug->EnableDebugLayer();
			D3DDebug->Release();
		}
#endif

		const HRESULT CreateFactRes = CreateDXGIFactory(IID_PPV_ARGS(&m_GiFactory));
		if (FAILED(CreateFactRes) || !m_GiFactory)
		{
			DeinitDevice();
			return;
		}

		m_GiAdapter = PickAdapter();

		// Device and Context:
		const HRESULT CreateDevRes = D3D12CreateDevice(
			m_GiAdapter,
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&m_D3DDevice));

		if (FAILED(CreateDevRes) || !m_D3DDevice)
		{
			DeinitDevice();
			return;
		}
	}

	// Command Queue:
	{
		D3D12_COMMAND_QUEUE_DESC Cqd = { };
		Cqd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		Cqd.Priority = 0;
		Cqd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		Cqd.NodeMask = 0;

		const HRESULT CreateCcRes = m_D3DDevice->CreateCommandQueue(&Cqd, IID_PPV_ARGS(&m_CommandQueue));
		if (FAILED(CreateCcRes) || !m_CommandQueue)
		{
			DeinitDevice();
			return;
		}
	}

	// Fence for swap chain:
	{
		const HRESULT Res = m_D3DDevice->CreateFence(m_SwapChainFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_SwapChainFence));
		if (FAILED(Res) || !m_SwapChainFence)
		{
			DeinitDevice();
			return;
		}

		m_SwapChainFenceEventHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!m_SwapChainFenceEventHandle)
		{
			DeinitDevice();
			return;
		}
	}

	// Swap Chain:
	{
		DXGI_SWAP_CHAIN_DESC1 ScDesc = { };
		ScDesc.BufferCount = 2;
		ScDesc.Width = m_ViewWidth;
		ScDesc.Height = m_ViewHeight;
		ScDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		ScDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		ScDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		ScDesc.SampleDesc.Quality = 0;
		ScDesc.SampleDesc.Count = 1;
		ScDesc.Scaling = DXGI_SCALING_STRETCH;
		ScDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		ScDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		IDXGISwapChain1* SwapChain1 = nullptr;
		const HRESULT CreateScRes = m_GiFactory->CreateSwapChainForHwnd(m_CommandQueue, m_TargetWnd, &ScDesc, nullptr, nullptr, &SwapChain1);
		if (SwapChain1)
		{
			SwapChain1->QueryInterface<IDXGISwapChain4>(&m_SwapChain);
			SwapChain1->Release();
		}
		if (FAILED(CreateScRes) || !m_SwapChain)
		{
			DeinitDevice();
			return;
		}

		m_GiFactory->MakeWindowAssociation(m_TargetWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

		m_CurrentFrameDataIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}

	if (!InitDescriptors())
	{
		DeinitDevice();
		return;
	}

	if (!InitBackBuffer())
	{
		DeinitDevice();
		return;
	}

	if (!InitDepthStencil())
	{
		DeinitDevice();
		return;
	}

	if (!InitSharedObjects())
	{
		DeinitDevice();
		return;
	}

	m_Shader = HWD3DRenderState_DX12::CreateRenderState(this, "_Media/DX12_VS.cso", "_Media/DX12_PS.cso");
}

void HWD3DGame_DX12::DeinitDevice()
{
	if (m_D3DDevice)
	{
		FlushSwapChain();
	}

	HWD3D_SafeRelease(m_RootSig);

	m_DepthStencilViewProvider.DestroyView(m_DepthStencilView);
	m_DepthStencilViewProvider.Deinit();
	HWD3D_SafeRelease(m_DepthStencilTexture);
	
	for (auto& FrameData : m_FrameData)
	{
		FrameData.Deinit(m_RenderTargetViewProvider);
	}
	m_RenderTargetViewProvider.Deinit();
	m_FrameData.resize(0);

	m_BufferViewProvider.Deinit();

	HWD3D_SafeRelease(m_Shader);
	HWD3D_SafeRelease(m_SwapChainCommandList);
	HWD3D_SafeRelease(m_SwapChain);
	if (m_SwapChainFenceEventHandle)
	{
		CloseHandle(m_SwapChainFenceEventHandle);
		m_SwapChainFenceEventHandle = 0;
	}
	HWD3D_SafeRelease(m_SwapChainFence);
	HWD3D_SafeRelease(m_CommandQueue);
	HWD3D_SafeRelease(m_D3DDevice);
	HWD3D_SafeRelease(m_GiAdapter);
	HWD3D_SafeRelease(m_GiFactory);
}

void HWD3DGame_DX12::ClearViewport()
{
	m_bShouldClearAtStartOfNextFrame = true;
}

bool HWD3DGame_DX12::BeginDraw()
{
	if (0 <= m_CurrentFrameDataIndex && m_CurrentFrameDataIndex < m_FrameData.size())
	{
		hwd3dFrameData& FrameData = m_FrameData[m_CurrentFrameDataIndex];
		SwapChainWaitForFenceValue(FrameData.FrameFenceValue);

		if (FrameData.CommandAlloc)
		{
			FrameData.CommandAlloc->Reset();
			m_SwapChainCommandList->Reset(FrameData.CommandAlloc, nullptr);
			m_CurrentFrameData = &FrameData;

			m_CurrentFrameData->PrepareToDraw(*m_SwapChainCommandList);

			const D3D12_CPU_DESCRIPTOR_HANDLE Rts[] = { m_CurrentFrameData->BufferDescriptor.CpuDescHandle };
			m_SwapChainCommandList->OMSetRenderTargets(_countof(Rts), Rts, FALSE, &m_DepthStencilView.CpuDescHandle);
			if (m_bShouldClearAtStartOfNextFrame)
			{
				m_bShouldClearAtStartOfNextFrame = false;
				const FLOAT ClearColor[] = { .4f , .4f , 1.f , 1.f };
				m_SwapChainCommandList->ClearRenderTargetView(m_CurrentFrameData->BufferDescriptor.CpuDescHandle, ClearColor, 0, NULL);
				m_SwapChainCommandList->ClearDepthStencilView(m_DepthStencilView.CpuDescHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, NULL);
			}

			//set the viewport
			D3D12_VIEWPORT Vp = { };
			Vp.Width = static_cast<FLOAT>(m_ViewWidth);
			Vp.Height = static_cast<FLOAT>(m_ViewHeight);
			Vp.MinDepth = 0.0f;
			Vp.MaxDepth = 1.0f;
			Vp.TopLeftX = 0;
			Vp.TopLeftY = 0;
			m_SwapChainCommandList->RSSetViewports(1, &Vp);

			D3D12_RECT ScissorRect = { };
			ScissorRect.left = 0;
			ScissorRect.top = 0;
			ScissorRect.right = m_ViewWidth;
			ScissorRect.bottom = m_ViewHeight;
			m_SwapChainCommandList->RSSetScissorRects(1, &ScissorRect);

			m_SwapChainCommandList->SetGraphicsRootSignature(m_RootSig);

			ID3D12DescriptorHeap* TextureHeaps[] = { m_BufferViewProvider.GetHeap() };
			m_SwapChainCommandList->SetDescriptorHeaps(_countof(TextureHeaps), TextureHeaps);

			if (m_Shader)
			{
				m_Shader->ApplyRenderState();
			}

			m_bConstantBufferDirty = true;
			m_CurrentFrameData->ConstantBuffer.BeginFrame();

			return true;
		}
	}

	return false;
}

void HWD3DGame_DX12::EndDraw()
{
	if (m_CurrentFrameData)
	{
		m_CurrentFrameData->PrepareToPresent(*m_SwapChainCommandList);
		m_SwapChainCommandList->Close();
		ID3D12CommandList* const CommandLists[] =
		{
			m_SwapChainCommandList ,
		};
		m_CommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);
		m_CurrentFrameData->FrameFenceValue = SwapChainSignal();
		m_CurrentFrameData = nullptr;
	}
}

void HWD3DGame_DX12::Present()
{
	if (m_SwapChain)
	{
		const bool bVSync = true;
		const DXGI_PRESENT_PARAMETERS Pp = { };
		const UINT PresentFlags = !bVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		m_SwapChain->Present1(bVSync ? 1 : 0, PresentFlags, &Pp);
		m_CurrentFrameDataIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}
}

void HWD3DGame_DX12::SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix)
{
	hwd3d_matrix* Mat = nullptr;

	switch (InType)
	{
	case HWD3DGame::hwd3d_transform_t::Proj:
		Mat = &m_Proj;
		break;
	case HWD3DGame::hwd3d_transform_t::View:
		Mat = &m_View;
		break;
	case HWD3DGame::hwd3d_transform_t::World:
		Mat = &m_World;
		break;
	}

	if (Mat)
	{
		*Mat = InMatrix;

		// Optimally we wouldn't set this every time a matrix changed, but for Hello World sample this is acceptable.
		m_ShaderWVP = HWD3DMatrix_Transpose(HWD3DMatrix_Multiply(m_World, HWD3DMatrix_Multiply(m_View, m_Proj)));
		m_bConstantBufferDirty = true;
	}
}

bool HWD3DGame_DX12::InitDescriptors()
{
	if (!m_D3DDevice)
	{
		return false;
	}

	m_RenderTargetViewProvider.Init(*m_D3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NUM_BACK_BUFFERS);
	m_DepthStencilViewProvider.Init(*m_D3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
	m_BufferViewProvider.Init(*m_D3DDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, NUM_RESOURCE_VIEWS);

	return true;
}

bool HWD3DGame_DX12::InitBackBuffer()
{
	if (!m_SwapChain || !m_D3DDevice)
	{
		return false;
	}

	m_FrameData.resize(NUM_BACK_BUFFERS);

	for (size_t BbIndex = 0; BbIndex < m_FrameData.size(); BbIndex++)
	{
		hwd3dFrameData& FrameData = m_FrameData[BbIndex];

		const HRESULT GetBufferRes = m_SwapChain->GetBuffer(BbIndex, IID_PPV_ARGS(&FrameData.BufferTexture));
		if (FAILED(GetBufferRes) || !FrameData.BufferTexture)
		{
			return false;
		}

		FrameData.BufferState = D3D12_RESOURCE_STATE_PRESENT; // Back buffers start in Present state.
		FrameData.BufferDescriptor = m_RenderTargetViewProvider.CreateView();
		m_D3DDevice->CreateRenderTargetView(FrameData.BufferTexture, nullptr, FrameData.BufferDescriptor.CpuDescHandle);

		FrameData.ConstantBuffer.Init(this, sizeof(m_ShaderWVP));

		// Every back buffer needs an allocator.
		const HRESULT CcaRes = m_D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&FrameData.CommandAlloc));
		if (FAILED(CcaRes) && !FrameData.CommandAlloc)
		{
			return false;
		}
	}

	// Single command list
	{
		// Initialize with frame 0's command allocator (this changes every frame, that's why we close it right away)
		const HRESULT CclRes = m_D3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameData[0].CommandAlloc, nullptr, IID_PPV_ARGS(&m_SwapChainCommandList));
		if (SUCCEEDED(CclRes) && m_SwapChainCommandList)
		{
			const HRESULT ClCloseRes = m_SwapChainCommandList->Close();
			assert(SUCCEEDED(ClCloseRes));
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool HWD3DGame_DX12::InitDepthStencil()
{
	D3D12_HEAP_PROPERTIES HeapProps = { };
	HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProps.CreationNodeMask = 0x1;
	HeapProps.VisibleNodeMask = 0x1;

	D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE;

	D3D12_RESOURCE_DESC RtvTd = { };
	RtvTd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	RtvTd.Width = m_ViewWidth;
	RtvTd.Height = m_ViewHeight;
	RtvTd.MipLevels = 1;
	RtvTd.DepthOrArraySize = 1;
	RtvTd.Format = DXGI_FORMAT_R16_TYPELESS;
	RtvTd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	RtvTd.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL|D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE; // If we wanted a readable depth stencil we wouldn't deny to shader resources.
	RtvTd.SampleDesc.Quality = 0;
	RtvTd.SampleDesc.Count = 1;

	D3D12_CLEAR_VALUE DefaultClearValue = { };
	DefaultClearValue.Format = DXGI_FORMAT_D16_UNORM;
	DefaultClearValue.DepthStencil.Depth = 1.f;
	DefaultClearValue.DepthStencil.Stencil = 0;

	m_DepthStencilState = D3D12_RESOURCE_STATE_DEPTH_WRITE; // Since we aren't reading from depth stencil, we can start in DEPTH_WRITE and never have to worry about transitions.
	const HRESULT CreateRtRes = m_D3DDevice->CreateCommittedResource(&HeapProps, HeapFlags, &RtvTd, m_DepthStencilState, &DefaultClearValue, IID_PPV_ARGS(&m_DepthStencilTexture));
	if (FAILED(CreateRtRes) || !m_DepthStencilTexture)
	{
		return false;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC RtvDesc = { };
	RtvDesc.Format = DXGI_FORMAT_D16_UNORM;
	RtvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	RtvDesc.Texture2D.MipSlice = 0;
	m_DepthStencilView = m_DepthStencilViewProvider.CreateView();
	if (!m_DepthStencilView.IsValid())
	{
		return false;
	}

	m_D3DDevice->CreateDepthStencilView(m_DepthStencilTexture, &RtvDesc, m_DepthStencilView.CpuDescHandle);

	return true;
}

bool HWD3DGame_DX12::InitSharedObjects()
{
	if (!m_D3DDevice)
	{
		return false;
	}

	// Root signature:
	{
		// We're using one static sampler, one texture register, and one constant buffer.

		D3D12_STATIC_SAMPLER_DESC Ssd = { };
		Ssd.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		Ssd.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		Ssd.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		Ssd.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		Ssd.MipLODBias = 0.f;
		Ssd.MaxAnisotropy = 16;
		Ssd.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		Ssd.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		Ssd.ShaderRegister = 0;
		Ssd.RegisterSpace = 0;
		Ssd.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		Ssd.MinLOD = 0.f;
		Ssd.MaxLOD = FLT_MAX;

		const D3D12_ROOT_SIGNATURE_FLAGS RootSigFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		D3D12_ROOT_SIGNATURE_DESC RootSigDesc = { };

		RootSigDesc.NumStaticSamplers = 1;
		RootSigDesc.pStaticSamplers = &Ssd;

		// When using vectors like this, we need to make sure the memory pointers are valid
		// (std::vector will move things around, so be very careful here.)

		std::vector<D3D12_ROOT_PARAMETER> RootParams;

		std::vector<D3D12_DESCRIPTOR_RANGE> TexTableRanges;

		{
			D3D12_DESCRIPTOR_RANGE TexRange;
			TexRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			TexRange.BaseShaderRegister =  0;
			TexRange.RegisterSpace = 0;
			TexRange.NumDescriptors = 1;
			TexRange.OffsetInDescriptorsFromTableStart = 0;
			TexTableRanges.push_back(TexRange);
		}

		// One constant buffer:
		{
			D3D12_ROOT_PARAMETER Rp = { };
			Rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			Rp.Descriptor.ShaderRegister = 0;
			Rp.Descriptor.RegisterSpace = 0;
			Rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			RootParams.push_back(Rp);
		}

		// One texture buffer:
		{
			D3D12_ROOT_PARAMETER Rp = { };
			Rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			Rp.DescriptorTable.NumDescriptorRanges = TexTableRanges.size();
			Rp.DescriptorTable.pDescriptorRanges = TexTableRanges.data();
			Rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			RootParams.push_back(Rp);
		}

		RootSigDesc.NumParameters = RootParams.size();
		RootSigDesc.pParameters = RootParams.data();

		RootSigDesc.Flags = RootSigFlags;

		ID3DBlob* SignatureBlob = nullptr;
		ID3DBlob* ErrorBlob = nullptr;
		const HRESULT CreateBlobRes = D3D12SerializeRootSignature(&RootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &SignatureBlob, &ErrorBlob);
		if (FAILED(CreateBlobRes) || !SignatureBlob)
		{
			HWD3D_SafeRelease(SignatureBlob);
			HWD3D_SafeRelease(ErrorBlob);
			return false;
		}

		const HRESULT CreateRes = m_D3DDevice->CreateRootSignature(0, SignatureBlob->GetBufferPointer(), SignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSig));
		
		HWD3D_SafeRelease(SignatureBlob);
		HWD3D_SafeRelease(ErrorBlob);

		if (FAILED(CreateRes) || !m_RootSig)
		{
			return false;
		}
	}

	return true;
}

void HWD3DGame_DX12::FlushSwapChain()
{
	SwapChainSignal();
	SwapChainWaitForFenceValue(m_SwapChainFenceValue);
}

UINT64 HWD3DGame_DX12::SwapChainSignal()
{
	if (m_CommandQueue && m_SwapChainFence)
	{
		const UINT64 FenceValueForSignal = ++m_SwapChainFenceValue;
		const HRESULT SigRes = m_CommandQueue->Signal(m_SwapChainFence, FenceValueForSignal);
		assert(SUCCEEDED(SigRes));
		return FenceValueForSignal;
	}
	else
	{
		assert(false);
	}
	return 0;
}

void HWD3DGame_DX12::SwapChainWaitForFenceValue(UINT64 InValue)
{
	if (m_SwapChainFence && m_SwapChainFenceEventHandle)
	{
		if (m_SwapChainFence->GetCompletedValue() < InValue)
		{
			m_SwapChainFence->SetEventOnCompletion(InValue, m_SwapChainFenceEventHandle);
			::WaitForSingleObject(m_SwapChainFenceEventHandle, INFINITE);
		}
	}
}

IDXGIAdapter* HWD3DGame_DX12::PickAdapter()
{
	if (!m_GiFactory)
	{
		return nullptr;
	}

	IDXGIAdapter* PotentialAdapter = nullptr;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_GiFactory->EnumAdapters(i, &PotentialAdapter); i++)
	{
		DXGI_ADAPTER_DESC AdapterDesc = { };
		PotentialAdapter->GetDesc(&AdapterDesc);

		// We'll actually just take the first adapter we find:
		if (i == 0)
		{
			return PotentialAdapter;
		}

		HWD3D_SafeRelease(PotentialAdapter);
	}

	return nullptr;
}