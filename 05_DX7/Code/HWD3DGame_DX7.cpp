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
		BbSd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
		BbSd.dwWidth = ScreenWidth;
		BbSd.dwHeight = ScreenHeight;
		const HRESULT CreateBbRes = m_DDraw->CreateSurface(&BbSd, &m_BackBuffer, 0);
		if (FAILED(CreateBbRes) || !m_BackBuffer)
		{
			Deinit();
			return;
		}
	}

	// Get device from back surface
	{
		// Z Buffer (Should be attached to back buffer before device is created.)
		{
			DDPIXELFORMAT ZBufferFormat = { };
			const HRESULT EnumZBufFormats = m_D3D->EnumZBufferFormats(m_DevicesFound.back().Id, D3DCb_EnumZBufferFormat, reinterpret_cast<VOID*>(&ZBufferFormat) );

			DDSURFACEDESC2 ZbSd = {};
			ZbSd.dwSize = sizeof(ZbSd);
			ZbSd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
			ZbSd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
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

		const HRESULT CreateDevRes = m_D3D->CreateDevice(m_DevicesFound.back().Id, m_BackBuffer, &m_D3DDevice);
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

	// Set WVP Matrices
	{
		const hwd3d_matrix ProjMatrix = HWD3DMatrix_BuildPerspectiveFovLH(HWD3D_ToRad(90.f), (static_cast<float>(ScreenWidth)/ScreenHeight), .1f , 1000.f );
		const hwd3d_matrix ViewMatrix = HWD3DMatrix_BuildLookAtLH(hwd3d_vec3(0.f, 0.f, -25.f), hwd3d_vec3(0.f,0.f,0.f), hwd3d_vec3(0.f,1.f,0.f));

		D3DMATRIX Proj = *reinterpret_cast<const D3DMATRIX*>(&ProjMatrix);
		D3DMATRIX View = *reinterpret_cast<const D3DMATRIX*>(&ViewMatrix);
		D3DMATRIX World = *reinterpret_cast<const D3DMATRIX*>(&HWD3DMatrix_Ident);

		m_D3DDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &Proj);
		m_D3DDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &View);
		m_D3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &World);
	}

	InitCommonStates();
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

void HWD3DGame_DX7::SetWorldMatrix(const hwd3d_matrix& InMatrix)
{
	if (m_D3DDevice)
	{
		D3DMATRIX Mat = *reinterpret_cast<const D3DMATRIX*>(&InMatrix);
		m_D3DDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &Mat);
	}
}

void HWD3DGame_DX7::InitCommonStates()
{
	if (!m_D3D || !m_D3DDevice)
	{
		return;
	}
}

HRESULT CALLBACK HWD3DGame_DX7::D3DCb_EnumDevices(LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpD3DDeviceDesc, LPVOID lpContext)
{
	HWD3DGame_DX7* _this = reinterpret_cast<HWD3DGame_DX7*>(lpContext);

	D3DDEVICEDESC7 d1 = *lpD3DDeviceDesc;

	d3dDeviceData NewData;
	NewData.Name = lpDeviceName;
	NewData.Id = lpD3DDeviceDesc->deviceGUID;

	_this->m_DevicesFound.push_back(NewData);

	const DWORD WantedCaps = D3DDEVCAPS_EXECUTEVIDEOMEMORY | D3DDEVCAPS_TLVERTEXVIDEOMEMORY | D3DDEVCAPS_TEXTUREVIDEOMEMORY;

	if ((d1.dwDevCaps & WantedCaps) == WantedCaps)
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
