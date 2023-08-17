// D3D Hello World

#include "HWD3DGame_DX2.h"
#include "HWD3DMesh.h"
#include "HWD3DTexture.h"
#include "d3dmacs.h"

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
		Clipper->Release();
		Clipper = nullptr;
	}

	// Back Surface
	{
		DDSURFACEDESC BbSd = {};
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

	// Z Buffer
	{
		DDSURFACEDESC ZbSd = {};
		ZbSd.dwSize = sizeof(ZbSd);
		ZbSd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		ZbSd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
		ZbSd.dwWidth = ScreenWidth;
		ZbSd.dwHeight = ScreenHeight;

		DDPIXELFORMAT& Pxf = ZbSd.ddpfPixelFormat;
		Pxf.dwSize = sizeof(Pxf);
		Pxf.dwFlags = DDPF_ZBUFFER;
		Pxf.dwZBufferBitDepth = 24;

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
		D3DFINDDEVICERESULT FindDev = {};
		FindDev.dwSize = sizeof(FindDev);
		D3DFINDDEVICESEARCH SearchDev = {};
		SearchDev.dwSize = sizeof(SearchDev);
		SearchDev.guid = m_DevicesFound.back().Id;

		const HRESULT FindDevRes = m_D3D->FindDevice(&SearchDev, &FindDev);
		if (FAILED(FindDevRes))
		{
			Deinit();
			return;
		}

		const HRESULT QueryForDevRes = m_BackBuffer->QueryInterface(FindDev.guid, reinterpret_cast<LPVOID*>(&m_D3DDevice));
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

	// Set WVP Matrices
	{
		const hwd3d_matrix ProjMatrix = HWD3DMatrix_BuildPerspectiveFovLH(HWD3D_ToRad(90.f), (static_cast<float>(ScreenWidth)/ScreenHeight), .1f , 1000.f );
		const hwd3d_matrix ViewMatrix = HWD3DMatrix_BuildLookAtLH(hwd3d_vec3(0.f, 0.f, -25.f), hwd3d_vec3(0.f,0.f,0.f), hwd3d_vec3(0.f,1.f,0.f));

		D3DMATRIX Proj = *reinterpret_cast<const D3DMATRIX*>(&ProjMatrix);
		D3DMATRIX View = *reinterpret_cast<const D3DMATRIX*>(&ViewMatrix);
		D3DMATRIX World = *reinterpret_cast<const D3DMATRIX*>(&HWD3DMatrix_Ident);

		m_D3DDevice->SetMatrix(m_MatrixProj, &Proj);
		m_D3DDevice->SetMatrix(m_MatrixView, &View);
		m_D3DDevice->SetMatrix(m_MatrixWorld, &World);
	}
}

void HWD3DGame_DX2::DeinitDevice()
{
	auto SafeRelease = [](auto*& p ) -> void
	{
		if (p)
		{
			p->Release();
			p = nullptr;
		}
	};

	auto SafeDeleteMatrix = [this](auto& m) -> void
	{
		if (m_D3DDevice)
		{
			m_D3DDevice->DeleteMatrix(m);
			m = 0;
		}

	};

	SafeRelease(m_BgMaterial);
	m_BgMaterialHandle = 0;
	SafeRelease(m_Viewport);

	SafeDeleteMatrix(m_MatrixWorld);
	SafeDeleteMatrix(m_MatrixView);
	SafeDeleteMatrix(m_MatrixProj);

	SafeRelease(m_D3DDevice);
	SafeRelease(m_ZBuffer);
	SafeRelease(m_BackBuffer);
	SafeRelease(m_PrimarySurface);
	SafeRelease(m_D3D);
	SafeRelease(m_DDraw);
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

void HWD3DGame_DX2::SetWorldMatrix(const hwd3d_matrix& InMatrix)
{
	if (m_D3DDevice)
	{
		D3DMATRIX Mat = *reinterpret_cast<const D3DMATRIX*>(&InMatrix);
		m_D3DDevice->SetMatrix(m_MatrixWorld, &Mat);
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
	}

	// This is a one off so we don't keep any pointers.

	D3DEXECUTEBUFFERDESC ExecBufferDesc = { };
	ExecBufferDesc.dwSize = sizeof(ExecBufferDesc);
	ExecBufferDesc.dwFlags = D3DDEB_BUFSIZE;
	static const int NUM_INSTR = 2;
	static const int NUM_RENDER_STATES = 3;
	ExecBufferDesc.dwBufferSize = sizeof(D3DINSTRUCTION)*NUM_INSTR + sizeof(D3DSTATE)*NUM_RENDER_STATES;
	IDirect3DExecuteBuffer* ExecBuffer = nullptr;
	const HRESULT CreateExecBufferRes = m_D3DDevice->CreateExecuteBuffer(&ExecBufferDesc, &ExecBuffer, nullptr);
	if (FAILED(CreateExecBufferRes) || !ExecBuffer)
	{
		return;
	}

	if (SUCCEEDED(ExecBuffer->Lock(&ExecBufferDesc)))
	{
		memset(ExecBufferDesc.lpData, 0, ExecBufferDesc.dwBufferSize);

		LPVOID lpBufStart = ExecBufferDesc.lpData;
		LPVOID lpPointer = lpBufStart;
		LPVOID lpInsStart = lpPointer;

		// Transform setting could be handled by a one off command buffer, but to keep things simple we do it all here.
		OP_STATE_TRANSFORM(3, lpPointer);
		STATE_DATA(D3DTRANSFORMSTATE_PROJECTION, GetProjMatrixHandle(), lpPointer);
		STATE_DATA(D3DTRANSFORMSTATE_VIEW, GetViewMatrixHandle(), lpPointer);
		STATE_DATA(D3DTRANSFORMSTATE_WORLD, GetWorldMatrixHandle(), lpPointer);
		OP_EXIT(lpPointer);

		const HRESULT UnlockRes = ExecBuffer->Unlock();
		if (FAILED(UnlockRes))
		{
			ExecBuffer->Release();
			return;
		}

		D3DEXECUTEDATA ExecData = {};
		ExecData.dwSize = sizeof(ExecData);
		ExecData.dwInstructionOffset = (ULONG)((char*)lpInsStart - (char*)lpBufStart);
		ExecData.dwInstructionLength = (ULONG)((char*)lpPointer - (char*)lpInsStart);
		ExecData.dwVertexCount = 0;
		const HRESULT SetDataRes = ExecBuffer->SetExecuteData(&ExecData);
		if (FAILED(SetDataRes))
		{
			ExecBuffer->Release();
			return;
		}

		m_D3DDevice->BeginScene();
		m_D3DDevice->Execute(ExecBuffer, m_Viewport, D3DEXECUTE_CLIPPED);
		m_D3DDevice->EndScene();
	}

	ExecBuffer->Release();
}

HRESULT FAR PASCAL HWD3DGame_DX2::D3DCb_EnumDevices(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC DevDesc1, LPD3DDEVICEDESC DevDesc2, LPVOID Context)
{
	HWD3DGame_DX2* _this = reinterpret_cast<HWD3DGame_DX2*>(Context);

	D3DDEVICEDESC d1 = *DevDesc1;
	D3DDEVICEDESC d2 = *DevDesc2;

	d3dDeviceData NewData;
	NewData.Name = lpDeviceName;
	NewData.Id = *lpGuid;

	_this->m_DevicesFound.push_back(NewData);

	const DWORD WantedCaps = D3DDEVCAPS_EXECUTEVIDEOMEMORY | D3DDEVCAPS_TLVERTEXVIDEOMEMORY | D3DDEVCAPS_TEXTUREVIDEOMEMORY;

	if ((d1.dwDevCaps & WantedCaps) == WantedCaps)
	{
		return S_OK;
	}

	return S_FALSE;
}
