// D3D Hello World

#include "HWD3DGame_DX9_SM3.h"

#pragma comment(lib, "d3d9.lib")

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	SetWindowTextW(InMainWnd, L"Hello World D3D [DX9 SM3]");

	HWD3DGame* Out = new HWD3DGame_DX9_SM3;
	if (Out)
	{
		Out->Init(InMainWnd);
	}

	return Out;
}

void HWD3DGame_DX9_SM3::InitDevice(HWND TargetWnd)
{
	m_TargetWnd = TargetWnd;

	RECT TargetWndRc = {};
	GetClientRect(m_TargetWnd, &TargetWndRc);
	const int ScreenWidth = TargetWndRc.right - TargetWndRc.left;
	const int ScreenHeight = TargetWndRc.bottom - TargetWndRc.top;

	// Init DirectDraw and obtain Direct3D
	{
		m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
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
		m_PP.SwapEffect = D3DSWAPEFFECT_DISCARD;
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
	if (false) // Don't actually need to set the viewport since by default it is se to the full render target.
	{
		// Update Viewport
		D3DVIEWPORT9 Vp = { };
		Vp.X = 0UL;
		Vp.Y = 0UL;
		Vp.Width = m_PP.BackBufferWidth;
		Vp.Height = m_PP.BackBufferHeight;
		Vp.MinZ = 0.0f;
		Vp.MaxZ = 1.0f;

		const HRESULT ScvpRes = m_D3DDevice->SetViewport(&Vp);
		assert(SUCCEEDED(ScvpRes));
	}
}

void HWD3DGame_DX9_SM3::DeinitDevice()
{
	HWD3D_SafeRelease(m_D3DDevice);
	HWD3D_SafeRelease(m_ZBuffer);
	HWD3D_SafeRelease(m_BackBuffer);
	HWD3D_SafeRelease(m_PrimarySurface);
	HWD3D_SafeRelease(m_D3D);
}

void HWD3DGame_DX9_SM3::ClearViewport()
{
	if (m_D3DDevice)
	{
		const HRESULT Res = m_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFF6666FF, 1.f, 0);
		assert(SUCCEEDED(Res));
	}
}

bool HWD3DGame_DX9_SM3::BeginDraw()
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

void HWD3DGame_DX9_SM3::EndDraw()
{
	if (m_D3DDevice)
	{
		const HRESULT EndSceneRes = m_D3DDevice->EndScene();
		assert(SUCCEEDED(EndSceneRes));
	}
}

void HWD3DGame_DX9_SM3::Present()
{
	if (m_D3DDevice)
	{
		m_D3DDevice->Present(NULL, NULL, NULL, NULL);
	}
}

void HWD3DGame_DX9_SM3::SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix)
{
	if (m_D3DDevice)
	{
		D3DMATRIX Mat = *reinterpret_cast<const D3DMATRIX*>(&InMatrix);

		D3DTRANSFORMSTATETYPE Type = static_cast<D3DTRANSFORMSTATETYPE>(0);
		switch (InType)
		{
		case HWD3DGame::hwd3d_transform_t::Proj:
			Type = D3DTS_PROJECTION;
			break;
		case HWD3DGame::hwd3d_transform_t::View:
			Type = D3DTS_VIEW;
			break;
		case HWD3DGame::hwd3d_transform_t::World:
			Type = D3DTS_WORLD;
			break;
		}

		m_D3DDevice->SetTransform(Type, &Mat);
	}
}
