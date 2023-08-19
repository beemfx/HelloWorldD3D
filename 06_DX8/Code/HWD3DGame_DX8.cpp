// D3D Hello World

#include "HWD3DGame_DX8.h"

#pragma comment(lib, "d3d8.lib")

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	SetWindowTextW(InMainWnd, L"Hello World D3D [DX8]");

	HWD3DGame* Out = new HWD3DGame_DX8;
	if (Out)
	{
		Out->Init(InMainWnd);
	}

	return Out;
}

void HWD3DGame_DX8::InitDevice(HWND TargetWnd)
{
	m_TargetWnd = TargetWnd;

	RECT TargetWndRc = {};
	GetClientRect(m_TargetWnd, &TargetWndRc);
	const int ScreenWidth = TargetWndRc.right - TargetWndRc.left;
	const int ScreenHeight = TargetWndRc.bottom - TargetWndRc.top;

	// Init DirectDraw and obtain Direct3D
	{
		m_D3D = Direct3DCreate8(D3D_SDK_VERSION);
		if (!m_D3D)
		{
			Deinit();
			return;
		}

		m_DevType = D3DDEVTYPE_HAL;

		m_PP.BackBufferWidth = ScreenWidth;
		m_PP.BackBufferHeight = ScreenHeight;
		m_PP.BackBufferCount = 1;
		m_PP.BackBufferFormat = D3DFMT_X8R8G8B8; // Same as desktop since we're windowed.
		m_PP.EnableAutoDepthStencil = TRUE;
		m_PP.AutoDepthStencilFormat = D3DFMT_D16;
		m_PP.SwapEffect = D3DSWAPEFFECT_COPY;
		m_PP.Windowed = TRUE;
		m_PP.hDeviceWindow = m_TargetWnd;

		const HRESULT CreateHalRes = m_D3D->CreateDevice(D3DADAPTER_DEFAULT, m_DevType, m_TargetWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_PP, &m_D3DDevice);
		if (FAILED(CreateHalRes) || !m_D3DDevice)
		{
			m_DevType = D3DDEVTYPE_REF;
			const HRESULT CreateRefRes = m_D3D->CreateDevice(D3DADAPTER_DEFAULT, m_DevType, m_TargetWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_PP, &m_D3DDevice);
			if (FAILED(CreateRefRes) || !m_D3DDevice)
			{
				Deinit();
				return;
			}
		}
	}

	// Viewport
	{
		// Update Viewport
		D3DVIEWPORT8 Vp = { };
		Vp.X = 0UL;
		Vp.Y = 0UL;
		Vp.Width = ScreenWidth;
		Vp.Height = ScreenHeight;
		Vp.MinZ = 0.0f;
		Vp.MaxZ = 1.0f;

		const HRESULT ScvpRes = m_D3DDevice->SetViewport(&Vp);
		if (FAILED(ScvpRes))
		{
			Deinit();
			return;
		}
	}

	// Set WVP Matrices
	{
		const hwd3d_matrix ProjMatrix = HWD3DMatrix_BuildPerspectiveFovLH(HWD3D_ToRad(90.f), (static_cast<float>(ScreenWidth)/ScreenHeight), .1f , 1000.f );
		const hwd3d_matrix ViewMatrix = HWD3DMatrix_BuildLookAtLH(hwd3d_vec3(0.f, 0.f, -25.f), hwd3d_vec3(0.f,0.f,0.f), hwd3d_vec3(0.f,1.f,0.f));

		D3DMATRIX Proj = *reinterpret_cast<const D3DMATRIX*>(&ProjMatrix);
		D3DMATRIX View = *reinterpret_cast<const D3DMATRIX*>(&ViewMatrix);
		D3DMATRIX World = *reinterpret_cast<const D3DMATRIX*>(&HWD3DMatrix_Ident);

		m_D3DDevice->SetTransform(D3DTS_PROJECTION, &Proj);
		m_D3DDevice->SetTransform(D3DTS_VIEW, &View);
		m_D3DDevice->SetTransform(D3DTS_WORLD, &World);
	}

	InitCommonStates();
}

void HWD3DGame_DX8::DeinitDevice()
{
	HWD3D_SafeRelease(m_D3DDevice);
	HWD3D_SafeRelease(m_ZBuffer);
	HWD3D_SafeRelease(m_BackBuffer);
	HWD3D_SafeRelease(m_PrimarySurface);
	HWD3D_SafeRelease(m_D3D);
}

void HWD3DGame_DX8::ClearViewport()
{
	if (m_D3DDevice)
	{
		D3DVIEWPORT8 Vp = { };
		m_D3DDevice->GetViewport(&Vp);
		D3DRECT ClearRect = { 0, 0, static_cast<LONG>(Vp.Width), static_cast<LONG>(Vp.Height) };
		const HRESULT Res = m_D3DDevice->Clear(1, &ClearRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFF6666FF, 1.f, 0);
		assert(SUCCEEDED(Res));
	}
}

bool HWD3DGame_DX8::BeginDraw()
{
	if (m_D3DDevice)
	{
		if (SUCCEEDED(m_D3DDevice->BeginScene()))
		{
			return true;
		}
	}

	return false;
}

void HWD3DGame_DX8::EndDraw()
{
	if (m_D3DDevice)
	{
		const HRESULT EndSceneRes = m_D3DDevice->EndScene();
		assert(SUCCEEDED(EndSceneRes));
	}
}

void HWD3DGame_DX8::Present()
{
	if (m_D3DDevice)
	{
		m_D3DDevice->Present(NULL, NULL, NULL, NULL);
	}
}

void HWD3DGame_DX8::SetWorldMatrix(const hwd3d_matrix& InMatrix)
{
	if (m_D3DDevice)
	{
		D3DMATRIX Mat = *reinterpret_cast<const D3DMATRIX*>(&InMatrix);
		m_D3DDevice->SetTransform(D3DTS_WORLD, &Mat);
	}
}

void HWD3DGame_DX8::InitCommonStates()
{
	if (!m_D3D || !m_D3DDevice)
	{
		return;
	}
}
