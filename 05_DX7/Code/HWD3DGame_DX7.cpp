// D3D Hello World

#include "HWD3DGame_DX7.h"

#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	SetWindowTextW(InMainWnd, L"Hello World D3D [DX7]");

	HWD3DGame* Out = new HWD3DGame_DX7;
	if (Out)
	{
		Out->Init(InMainWnd);
	}

	return Out;
}

void HWD3DGame_DX7::InitDevice(HWND TargetWnd)
{
	m_TargetWnd = TargetWnd;
	m_MemFlag = 0;

	RECT TargetWndRc = {};
	GetClientRect(m_TargetWnd, &TargetWndRc);
	const int ScreenWidth = TargetWndRc.right - TargetWndRc.left;
	const int ScreenHeight = TargetWndRc.bottom - TargetWndRc.top;

	// Init DirectDraw and obtain Direct3D
	{
		const HRESULT CciRes = DirectDrawCreateEx(NULL, reinterpret_cast<LPVOID*>(&m_DDraw), IID_IDirectDraw7, NULL);
		if (FAILED(CciRes) || !m_DDraw)
		{
			Deinit();
			return;
		}
		
		const HRESULT SetCpLvlRes = m_DDraw->SetCooperativeLevel(TargetWnd, DDSCL_NORMAL);
		if (FAILED(SetCpLvlRes))
		{
			Deinit();
			return;
		}

		const HRESULT QueryD3DRes = m_DDraw->QueryInterface(IID_IDirect3D7, reinterpret_cast<LPVOID*>(&m_D3D));
		if (FAILED(QueryD3DRes) || !m_D3D)
		{
			Deinit();
			return;
		}

		const HRESULT EnumDevRes = m_D3D->EnumDevices(&D3DCb_EnumDevices, this);
		if (FAILED(EnumDevRes) || m_DevicesFound.size() == 0)
		{
			Deinit();
			return;
		}

		if ((m_DevicesFound.back().Desc.dwDevCaps&D3DDEVCAPS_TEXTUREVIDEOMEMORY) != 0)
		{
			m_MemFlag = DDSCAPS_VIDEOMEMORY;
		}
		else
		{
			m_MemFlag = DDSCAPS_SYSTEMMEMORY;
		}
	}

	// Primary Surface
	{
		DDSURFACEDESC2 BbSd = {};
		BbSd.dwSize = sizeof(BbSd);
		BbSd.dwFlags = DDSD_CAPS;
		BbSd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		const HRESULT CreateBbRes = m_DDraw->CreateSurface(&BbSd, &m_PrimarySurface, nullptr);
		if (FAILED(CreateBbRes) || !m_PrimarySurface)
		{
			Deinit();
			return;
		}
	}

	// Clipper
	{
		IDirectDrawClipper* Clipper = nullptr;
		const HRESULT CreateCRes = m_DDraw->CreateClipper(0, &Clipper, 0);
		if (FAILED(CreateCRes) || !Clipper)
		{
			Deinit();
			return;
		}

		Clipper->SetHWnd(0, TargetWnd);
		m_PrimarySurface->SetClipper(Clipper);
		HWD3D_SafeRelease(Clipper);
	}

	// Back Surface
	{
		DDSURFACEDESC2 BbSd = {};
		BbSd.dwSize = sizeof(BbSd);
		BbSd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		BbSd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | GetMemFlag();
		BbSd.dwWidth = ScreenWidth;
		BbSd.dwHeight = ScreenHeight;
		const HRESULT CreateBbRes = m_DDraw->CreateSurface(&BbSd, &m_BackBuffer, 0);
		if (FAILED(CreateBbRes) || !m_BackBuffer)
		{
			Deinit();
			return;
		}
	}

	// Z Buffer (Should be attached to back buffer before device is created.)
	{
		DDPIXELFORMAT ZBufferFormat = { };
		const HRESULT EnumZBufFormats = m_D3D->EnumZBufferFormats(m_DevicesFound.back().Desc.deviceGUID, D3DCb_EnumZBufferFormat, reinterpret_cast<VOID*>(&ZBufferFormat) );

		DDSURFACEDESC2 ZbSd = {};
		ZbSd.dwSize = sizeof(ZbSd);
		ZbSd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		ZbSd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | GetMemFlag();
		ZbSd.dwWidth = ScreenWidth;
		ZbSd.dwHeight = ScreenHeight;
		ZbSd.dwMipMapCount = 1;

		DDPIXELFORMAT& Pxf = ZbSd.ddpfPixelFormat;
		Pxf = ZBufferFormat;

		const HRESULT CreateZbRes = m_DDraw->CreateSurface(&ZbSd, &m_ZBuffer, 0);
		if (FAILED(CreateZbRes) || !m_ZBuffer)
		{
			Deinit();
			return;
		}

		const HRESULT AddZBufferRes = m_BackBuffer->AddAttachedSurface(m_ZBuffer);
		if (FAILED(AddZBufferRes))
		{
			Deinit();
			return;
		}
	}

	// Get device from back surface
	{
		const HRESULT CreateDevRes = m_D3D->CreateDevice(m_DevicesFound.back().Desc.deviceGUID, m_BackBuffer, &m_D3DDevice);
		if (FAILED(CreateDevRes) || !m_D3DDevice)
		{
			Deinit();
			return;
		}
	}

	// Viewport
	{
		// Update Viewport
		D3DVIEWPORT7 Vp = { };
		Vp.dwX = 0UL;
		Vp.dwY = 0UL;
		Vp.dwWidth = ScreenWidth;
		Vp.dwHeight = ScreenHeight;
		Vp.dvMinZ = 0.0f;
		Vp.dvMaxZ = 1.0f;

		const HRESULT ScvpRes = m_D3DDevice->SetViewport(&Vp);
		if (FAILED(ScvpRes))
		{
			Deinit();
			return;
		}
	}
}

void HWD3DGame_DX7::DeinitDevice()
{
	HWD3D_SafeRelease(m_D3DDevice);
	HWD3D_SafeRelease(m_ZBuffer);
	HWD3D_SafeRelease(m_BackBuffer);
	HWD3D_SafeRelease(m_PrimarySurface);
	HWD3D_SafeRelease(m_D3D);
	HWD3D_SafeRelease(m_DDraw);
}

void HWD3DGame_DX7::ClearViewport()
{
	if (m_D3DDevice)
	{
		D3DVIEWPORT7 Vp = { };
		m_D3DDevice->GetViewport(&Vp);
		D3DRECT ClearRect = { 0, 0, static_cast<LONG>(Vp.dwWidth), static_cast<LONG>(Vp.dwHeight) };
		const HRESULT Res = m_D3DDevice->Clear(1, &ClearRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, RGBA_SETALPHA(RGB_MAKE(102, 102, 255), 255), 1.f, 0);
		assert(SUCCEEDED(Res));
	}
}

bool HWD3DGame_DX7::BeginDraw()
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

void HWD3DGame_DX7::EndDraw()
{
	if (m_D3DDevice)
	{
		const HRESULT EndSceneRes = m_D3DDevice->EndScene();
		assert(SUCCEEDED(EndSceneRes));
	}
}

void HWD3DGame_DX7::Present()
{
	if (m_DDraw && m_BackBuffer)
	{
		DDSURFACEDESC2 desc = {};
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT;
		m_BackBuffer->GetSurfaceDesc(&desc);

		RECT rcSource = {};
		rcSource.top = rcSource.left = 0;
		rcSource.bottom = desc.dwHeight;
		rcSource.right = desc.dwWidth;

		RECT RcWnd = {};
		GetClientRect(m_TargetWnd, &RcWnd);
		POINT Offset = {};
		ClientToScreen(m_TargetWnd, &Offset);
		OffsetRect(&RcWnd, Offset.x, Offset.y);

		const HRESULT Res = m_PrimarySurface->Blt(&RcWnd, m_BackBuffer, &rcSource, DDBLT_WAIT, 0);
		if (Res == DDERR_SURFACELOST)
		{
			// If the surface was lost we'd need to attempt to restore it at some point, but
			// we're keeping things simple (and in windows mode, surfaces don't generally
			// get lost.
			return;
		}
	}
}

void HWD3DGame_DX7::SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix)
{
	if (m_D3DDevice)
	{
		D3DMATRIX Mat = *reinterpret_cast<const D3DMATRIX*>(&InMatrix);
		
		D3DTRANSFORMSTATETYPE Type = static_cast<D3DTRANSFORMSTATETYPE>(0);
		switch (InType)
		{
		case HWD3DGame::hwd3d_transform_t::Proj:
			Type = D3DTRANSFORMSTATE_PROJECTION;
			break;
		case HWD3DGame::hwd3d_transform_t::View:
			Type = D3DTRANSFORMSTATE_VIEW;
			break;
		case HWD3DGame::hwd3d_transform_t::World:
			Type = D3DTRANSFORMSTATE_WORLD;
			break;
		}

		m_D3DDevice->SetTransform(Type, &Mat);
	}
}

HRESULT CALLBACK HWD3DGame_DX7::D3DCb_EnumDevices(LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpD3DDeviceDesc, LPVOID lpContext)
{
	HWD3DGame_DX7* _this = reinterpret_cast<HWD3DGame_DX7*>(lpContext);

	d3dDeviceData NewData;
	NewData.Desc = *lpD3DDeviceDesc;

	_this->m_DevicesFound.push_back(NewData);

	const DWORD WantedCaps = D3DDEVCAPS_EXECUTEVIDEOMEMORY | D3DDEVCAPS_TLVERTEXVIDEOMEMORY | D3DDEVCAPS_TEXTUREVIDEOMEMORY;

	if ((lpD3DDeviceDesc->dwDevCaps & WantedCaps) == WantedCaps)
	{
		return S_OK;
	}

	return S_FALSE;
}

HRESULT WINAPI HWD3DGame_DX7::D3DCb_EnumZBufferFormat(DDPIXELFORMAT* pddpf, VOID* pddpfDesired)
{
	if( pddpf->dwFlags == (DDPF_ZBUFFER) )
	{
		*reinterpret_cast<DDPIXELFORMAT*>(pddpfDesired) = *pddpf;
		return D3DENUMRET_CANCEL;
	}

	return D3DENUMRET_OK;
}
