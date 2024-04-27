// D3D Hello World

#include "HWD3DGame_DX2.h"
#include "HWD3DMesh.h"
#include "HWD3DTexture.h"
#include "HWD3DExecBuffer_DX2.h"

// #pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	SetWindowTextW(InMainWnd, L"Hello World D3D [DX2]");

	HWD3DGame* Out = new HWD3DGame_DX2;
	if (Out)
	{
		Out->Init(InMainWnd);
	}

	return Out;
}

void HWD3DGame_DX2::InitDevice(HWND TargetWnd)
{
	m_TargetWnd = TargetWnd;
	m_MemFlag = 0;

	RECT TargetWndRc = {};
	GetClientRect(m_TargetWnd, &TargetWndRc);
	const int ScreenWidth = TargetWndRc.right - TargetWndRc.left;
	const int ScreenHeight = TargetWndRc.bottom - TargetWndRc.top;

	// Init DirectDraw and obtain Direct3D
	{
		// Don't actually seem to need this: HRESULT CoInitRes = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		const HRESULT CciRes = CoCreateInstance(CLSID_DirectDraw, NULL, CLSCTX_ALL, IID_IDirectDraw, reinterpret_cast<LPVOID*>(&m_DDraw));

		if (FAILED(CciRes) || !m_DDraw)
		{
			Deinit();
			return;
		}

		const HRESULT InitRes = m_DDraw->Initialize(nullptr);
		if (FAILED(InitRes))
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

		const HRESULT QueryD3DRes = m_DDraw->QueryInterface(IID_IDirect3D, reinterpret_cast<LPVOID*>(&m_D3D));
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

		if (m_DevicesFound.back().bUseVideoMem)
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
		DDSURFACEDESC BbSd = {};
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
		DDSURFACEDESC BbSd = {};
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

	// Z Buffer
	{
		DDSURFACEDESC ZbSd = {};
		ZbSd.dwSize = sizeof(ZbSd);
		ZbSd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		ZbSd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | GetMemFlag();
		ZbSd.dwWidth = ScreenWidth;
		ZbSd.dwHeight = ScreenHeight;

		DDPIXELFORMAT& Pxf = ZbSd.ddpfPixelFormat;
		Pxf.dwSize = sizeof(Pxf);
		Pxf.dwFlags = DDPF_ZBUFFER;
		Pxf.dwZBufferBitDepth = 16;

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
		const HRESULT QueryForDevRes = m_BackBuffer->QueryInterface(m_DevicesFound.back().Id, reinterpret_cast<LPVOID*>(&m_D3DDevice));
		if (FAILED(QueryForDevRes) || !m_D3DDevice)
		{
			Deinit();
			return;
		}
	}

	// Viewport
	{
		const HRESULT CvpRes = m_D3D->CreateViewport(&m_Viewport, nullptr);
		if (FAILED(CvpRes) || !m_Viewport)
		{
			Deinit();
			return;
		}
		const HRESULT AvpRes = m_D3DDevice->AddViewport(m_Viewport);
		if (FAILED(AvpRes))
		{
			Deinit();
			return;
		}

		D3DVIEWPORT Vp = {};
		Vp.dwSize = sizeof(Vp);
		Vp.dwX = Vp.dwY = 0;
		Vp.dwWidth = ScreenWidth;
		Vp.dwHeight = ScreenHeight;
		Vp.dvScaleX = Vp.dwWidth / 2.f;
		Vp.dvScaleY = Vp.dwHeight / 2.f;
		Vp.dvMaxX = D3DVAL(Vp.dwWidth) / D3DVAL(2.f * Vp.dvScaleX);
		Vp.dvMaxY = D3DVAL(Vp.dwHeight) / D3DVAL(2.f * Vp.dvScaleY);
		Vp.dvMaxZ = 1.f;
		const HRESULT SvpRes = m_Viewport->SetViewport(&Vp);
		if (FAILED(SvpRes))
		{
			Deinit();
			return;
		}
	}

	// Background Material
	{
		const HRESULT CmRes = m_D3D->CreateMaterial(&m_BgMaterial, nullptr);
		if (FAILED(CmRes) || !m_BgMaterial)
		{
			Deinit();
			return;
		}

		D3DMATERIAL MaterialDesc = {};
		MaterialDesc.dwSize = sizeof(MaterialDesc);
		// Bluish color
		MaterialDesc.diffuse.r = .4f;
		MaterialDesc.diffuse.g = .4f;
		MaterialDesc.diffuse.b = 1.f;
		MaterialDesc.diffuse.a = 1.f;
		MaterialDesc.ambient.r = 1.f;
		MaterialDesc.ambient.g = 1.f;
		MaterialDesc.ambient.b = 1.f;
		MaterialDesc.ambient.a = 1.f;
		MaterialDesc.specular.r = 1.f;
		MaterialDesc.specular.g = 1.f;
		MaterialDesc.specular.b = 1.f;
		MaterialDesc.specular.a = 1.f;
		MaterialDesc.power = 1.f;
		MaterialDesc.dwRampSize = 1;
		MaterialDesc.hTexture = 0;
		const HRESULT SmRes = m_BgMaterial->SetMaterial(&MaterialDesc);
		if (FAILED(SmRes))
		{
			Deinit();
			return;
		}
		const HRESULT GhRes = m_BgMaterial->GetHandle(m_D3DDevice, &m_BgMaterialHandle);
		if (FAILED(GhRes) || m_BgMaterialHandle == 0)
		{
			Deinit();
			return;
		}

		m_Viewport->SetBackground(m_BgMaterialHandle);
	}

	InitCommonStates();
}

void HWD3DGame_DX2::DeinitDevice()
{
	auto SafeDeleteMatrix = [this](auto& m) -> void
	{
		if (m_D3DDevice)
		{
			m_D3DDevice->DeleteMatrix(m);
			m = 0;
		}
	};

	HWD3D_SafeRelease(m_BgMaterial);
	m_BgMaterialHandle = 0;
	HWD3D_SafeRelease(m_Viewport);

	SafeDeleteMatrix(m_MatrixWorld);
	SafeDeleteMatrix(m_MatrixView);
	SafeDeleteMatrix(m_MatrixProj);

	HWD3D_SafeRelease(m_D3DDevice);
	HWD3D_SafeRelease(m_ZBuffer);
	HWD3D_SafeRelease(m_BackBuffer);
	HWD3D_SafeRelease(m_PrimarySurface);
	HWD3D_SafeRelease(m_D3D);
	HWD3D_SafeRelease(m_DDraw);
}

void HWD3DGame_DX2::ClearViewport()
{
	if (m_Viewport)
	{
		D3DVIEWPORT Vp = {};
		Vp.dwSize = sizeof(Vp);
		m_Viewport->GetViewport(&Vp);
		D3DRECT ClearRect = { 0, 0, static_cast<LONG>(Vp.dwWidth), static_cast<LONG>(Vp.dwHeight) };
		m_Viewport->Clear(1, &ClearRect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);
	}
}

bool HWD3DGame_DX2::BeginDraw()
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

void HWD3DGame_DX2::EndDraw()
{
	if (m_D3DDevice)
	{
		const HRESULT EndSceneRes = m_D3DDevice->EndScene();
		assert(SUCCEEDED(EndSceneRes));

		// Can use the following to obtain the dirty rectangle, but to keep things simple we
		// blit the whole back buffer:
		// 
		// D3DEXECUTEDATA ExecData = {};
		// ExecData.dwSize = sizeof(ExecData);
		// const HRESULT GetExecDataRes = m_ExecBuffer->GetExecuteData(&ExecData);
		// assert(SUCCEEDED(GetExecDataRes));
	}
}

void HWD3DGame_DX2::Present()
{
	if (m_DDraw && m_BackBuffer)
	{
		DDSURFACEDESC desc = {};
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

void HWD3DGame_DX2::SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix)
{
	if (m_D3DDevice)
	{
		D3DMATRIX Mat = *reinterpret_cast<const D3DMATRIX*>(&InMatrix);

		D3DMATRIXHANDLE Type = 0;

		switch (InType)
		{
		case HWD3DGame::hwd3d_transform_t::Proj:
			Type = m_MatrixProj;
			break;
		case HWD3DGame::hwd3d_transform_t::View:
			Type = m_MatrixView;
			break;
		case HWD3DGame::hwd3d_transform_t::World:
			Type = m_MatrixWorld;
			break;
		}

		m_D3DDevice->SetMatrix(Type, &Mat);
	}
}

void HWD3DGame_DX2::InitCommonStates()
{
	if (!m_D3D || !m_D3DDevice)
	{
		return;
	}

	assert(!m_MatrixProj && !m_MatrixView && !m_MatrixWorld);

	// WVP Matrices
	{
		m_D3DDevice->CreateMatrix(&m_MatrixProj);
		m_D3DDevice->CreateMatrix(&m_MatrixView);
		m_D3DDevice->CreateMatrix(&m_MatrixWorld);

		D3DMATRIX I = *reinterpret_cast<const D3DMATRIX*>(&HWD3DMatrix_Ident);

		m_D3DDevice->SetMatrix(m_MatrixProj, &I );
		m_D3DDevice->SetMatrix(m_MatrixView, &I );
		m_D3DDevice->SetMatrix(m_MatrixWorld, &I );
	}

	// This is a one off so we don't keep any pointers.
	if (HWD3DExecBuffer_DX2* ExecBuffer = new HWD3DExecBuffer_DX2(this))
	{
		// Transform setting could be handled by a one off command buffer, but to keep things simple we do it all here.
		ExecBuffer->BeginData();
		ExecBuffer->BeginInstructions();
		ExecBuffer->OP_STATE_TRANSFORM(3);
		ExecBuffer->STATE_DATA(D3DTRANSFORMSTATE_PROJECTION, GetProjMatrixHandle());
		ExecBuffer->STATE_DATA(D3DTRANSFORMSTATE_VIEW, GetViewMatrixHandle());
		ExecBuffer->STATE_DATA(D3DTRANSFORMSTATE_WORLD, GetWorldMatrixHandle());
		ExecBuffer->FinalizeBuffer();

		m_D3DDevice->BeginScene();
		ExecBuffer->ExecuteBuffer();
		m_D3DDevice->EndScene();

		HWD3D_SafeRelease(ExecBuffer);
	}
}

HRESULT FAR PASCAL HWD3DGame_DX2::D3DCb_EnumDevices(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC DevDesc1, LPD3DDEVICEDESC DevDesc2, LPVOID Context)
{
	HWD3DGame_DX2* _this = reinterpret_cast<HWD3DGame_DX2*>(Context);

	D3DDEVICEDESC d1 = *DevDesc1;
	D3DDEVICEDESC d2 = *DevDesc2;

	const DWORD WantedCaps = D3DDEVCAPS_EXECUTEVIDEOMEMORY | D3DDEVCAPS_TLVERTEXVIDEOMEMORY | D3DDEVCAPS_TEXTUREVIDEOMEMORY;

	d3dDeviceData NewData;
	NewData.Name = lpDeviceName;
	NewData.Id = *lpGuid;
	NewData.bUseVideoMem = (d1.dwDevCaps & WantedCaps) == WantedCaps;

	_this->m_DevicesFound.push_back(NewData);


	if (NewData.bUseVideoMem)
	{	
		return S_OK;
	}

	return S_FALSE;
}
