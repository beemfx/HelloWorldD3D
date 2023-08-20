// D3D Hello World

#include "HWD3DGame_DX10.h"
#include "HWD3DShader_DX10.h"

#pragma comment(lib, "d3d10.lib")

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	SetWindowTextW(InMainWnd, L"Hello World D3D [DX10]");

	HWD3DGame* Out = new HWD3DGame_DX10;
	if (Out)
	{
		Out->Init(InMainWnd);
	}

	return Out;
}

void HWD3DGame_DX10::InitDevice(HWND TargetWnd)
{
	m_TargetWnd = TargetWnd;

	RECT TargetWndRc = {};
	GetClientRect(m_TargetWnd, &TargetWndRc);
	m_ViewWidth = TargetWndRc.right - TargetWndRc.left;
	m_ViewHeight = TargetWndRc.bottom - TargetWndRc.top;

	// Init DirectDraw and obtain Direct3D
	{
		DXGI_SWAP_CHAIN_DESC ScDesc = { };
		ScDesc.BufferCount = 2;
		ScDesc.Windowed = TRUE;
		ScDesc.OutputWindow = m_TargetWnd;
		ScDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		ScDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		ScDesc.SampleDesc.Quality = 0;
		ScDesc.SampleDesc.Count = 1;
		ScDesc.BufferDesc.Width = m_ViewWidth;
		ScDesc.BufferDesc.Height = m_ViewHeight;
		ScDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		ScDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		ScDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;

		DWORD Flags = 0;

#if defined(_DEBUG)
		Flags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

		const HRESULT CreateDevRes = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, Flags, D3D10_SDK_VERSION, &ScDesc, &m_SwapChain, &m_D3DDevice);
		if (FAILED(CreateDevRes) || !m_D3DDevice || !m_SwapChain)
		{
			Deinit();
			return;
		}
	}

	// Back Buffer:
	{
		const HRESULT GetBbRes = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&m_RTVTexture));
		if (FAILED(GetBbRes) || !m_RTVTexture)
		{
			return;
		}

		D3D10_RENDER_TARGET_VIEW_DESC RtvDesc = { };
		RtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		RtvDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
		RtvDesc.Texture2D.MipSlice = 0;
		const HRESULT CreateRtvRes = m_D3DDevice->CreateRenderTargetView(m_RTVTexture, &RtvDesc, &m_RTV);
		if (FAILED(CreateRtvRes) || !m_RTV)
		{
			Deinit();
			return;
		}
	}

	// Z-Buffer
	{
		D3D10_TEXTURE2D_DESC RtvTd = { };
		RtvTd.Width = m_ViewWidth;
		RtvTd.Height = m_ViewHeight;
		RtvTd.MipLevels = 1;
		RtvTd.ArraySize = 1;
		RtvTd.Format = DXGI_FORMAT_R16_TYPELESS;
		RtvTd.Usage = D3D10_USAGE_DEFAULT;
		RtvTd.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		RtvTd.SampleDesc.Quality = 0;
		RtvTd.SampleDesc.Count = 1;
		const HRESULT CreateRtRes = m_D3DDevice->CreateTexture2D(&RtvTd,  NULL, &m_DSVTexture);
		if (FAILED(CreateRtRes) || !m_DSVTexture)
		{
			Deinit();
			return;
		}

		D3D10_DEPTH_STENCIL_VIEW_DESC RtvDesc = { };
		RtvDesc.Format = DXGI_FORMAT_D16_UNORM;
		RtvDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		RtvDesc.Texture2D.MipSlice = 0;
		const HRESULT CreateRtvRes = m_D3DDevice->CreateDepthStencilView(m_DSVTexture, &RtvDesc, &m_DSV);
		if (FAILED(CreateRtvRes) || !m_DSV)
		{
			Deinit();
			return;
		}
	}

	if (!CreateConstBuffer())
	{
		Deinit();
		return;
	}
	m_Shader = HWD3DShader_DX10::CreateShader(this, "_Media/DX10_VS.cso", "_Media/DX10_PS.cso");
}

void HWD3DGame_DX10::DeinitDevice()
{
	HWD3D_SafeRelease(m_Shader);
	HWD3D_SafeRelease(m_SS);
	HWD3D_SafeRelease(m_RS);
	HWD3D_SafeRelease(m_VSConstBuffer);
	HWD3D_SafeRelease(m_DSV);
	HWD3D_SafeRelease(m_DSVTexture);
	HWD3D_SafeRelease(m_RTV);
	HWD3D_SafeRelease(m_RTVTexture);
	HWD3D_SafeRelease(m_SwapChain);
	HWD3D_SafeRelease(m_D3DDevice);
}

void HWD3DGame_DX10::ClearViewport()
{
	if (m_D3DDevice && m_RTV && m_DSV)
	{
		const FLOAT ClearColor[] = { .4f , .4f , 1.f , 1.f };
		m_D3DDevice->ClearRenderTargetView(m_RTV, ClearColor);
		m_D3DDevice->ClearDepthStencilView(m_DSV, D3D10_CLEAR_DEPTH , 1.f, 0);
	}
}

bool HWD3DGame_DX10::BeginDraw()
{
	if (m_D3DDevice)
	{
		m_D3DDevice->OMSetRenderTargets(1, &m_RTV, m_DSV);

		//set the viewport
		D3D10_VIEWPORT Vp = { };
		Vp.Width = m_ViewWidth;
		Vp.Height = m_ViewHeight;
		Vp.MinDepth = 0.0f;
		Vp.MaxDepth = 1.0f;
		Vp.TopLeftX = 0;
		Vp.TopLeftY = 0;
		m_D3DDevice->RSSetViewports(1, &Vp);

		m_D3DDevice->RSSetState(m_RS);
		m_D3DDevice->PSSetSamplers(0, 1, &m_SS);

		if (m_Shader)
		{
			m_Shader->SetShader();
		}

		return true;
	}

	return false;
}

void HWD3DGame_DX10::EndDraw()
{
	if (m_D3DDevice)
	{
		
	}
}

void HWD3DGame_DX10::Present()
{
	if (m_SwapChain)
	{
		m_SwapChain->Present(0, 0);
	}
}

void HWD3DGame_DX10::SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix)
{
	if (m_D3DDevice)
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

			if (m_VSConstBuffer)
			{
				void* Dest = nullptr;
				const HRESULT MapRes = m_VSConstBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &Dest);
				if (SUCCEEDED(MapRes) && Dest)
				{
					memcpy(Dest, &m_ShaderWVP, sizeof(m_ShaderWVP));
					m_VSConstBuffer->Unmap();
				}

				m_D3DDevice->VSSetConstantBuffers(0, 1, &m_VSConstBuffer);
			}
		}
	}
}

bool HWD3DGame_DX10::CreateConstBuffer()
{
	if (!m_D3DDevice)
	{
		return false;
	}

	// Constant Buffer
	{
		D3D10_BUFFER_DESC Bd = { };
		Bd.ByteWidth           = sizeof(m_ShaderWVP);
		Bd.Usage               = D3D10_USAGE_DYNAMIC;
		Bd.BindFlags           = D3D10_BIND_CONSTANT_BUFFER;
		Bd.CPUAccessFlags      = D3D10_CPU_ACCESS_WRITE;
		Bd.MiscFlags           = 0;
		const HRESULT CreateBufferRes = m_D3DDevice->CreateBuffer(&Bd, NULL, &m_VSConstBuffer);
		if (FAILED(CreateBufferRes) || !m_VSConstBuffer)
		{
			return false;
		}
	}

	// Raster State:
	{
		D3D10_RASTERIZER_DESC Rsd = { };
		Rsd.CullMode = D3D10_CULL_NONE;
		Rsd.FillMode = D3D10_FILL_SOLID;
		Rsd.FrontCounterClockwise = true;
		Rsd.DepthBias = false;
		Rsd.DepthBiasClamp = 0;
		Rsd.SlopeScaledDepthBias = 0;
		Rsd.DepthClipEnable = true;
		Rsd.ScissorEnable = false;
		Rsd.MultisampleEnable = false;
		Rsd.AntialiasedLineEnable = true;
		const HRESULT Res = m_D3DDevice->CreateRasterizerState(&Rsd, &m_RS);
		if (FAILED(Res) || !m_RS)
		{
			return false;
		}
	}

	// Sampler State:
	{
		D3D10_SAMPLER_DESC Ssd = { };
		Ssd.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		Ssd.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		Ssd.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		Ssd.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
		const HRESULT Res = m_D3DDevice->CreateSamplerState(&Ssd, &m_SS);
		if (FAILED(Res) || !m_SS)
		{
			return false;
		}
	}

	return true;
}
