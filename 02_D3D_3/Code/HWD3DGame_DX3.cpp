// D3D Hello World

#include "HWD3DGame_DX3.h"
#include "d3dmacs.h"

// #pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	HWD3DGame* Out = new HWD3DGame_DX3;
	if (Out)
	{
		Out->Init(InMainWnd);
	}

	return Out;
}

static const D3DMATRIX DHWG_Proj =
{
	D3DVAL(2.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(2.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0)
};

static const D3DMATRIX DHWG_View =
{
	D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(10.0), D3DVAL(1.0)
};

static const D3DMATRIX DHWG_Ident =
{
	D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0)
};

/* Cube vertices, normals, shades, and modeling transform */
static int NumVertices = 24;
static D3DVERTEX CubeVertices[] = {
	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0) },
	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0) },

	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0) },
	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0) },

	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0) },
	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0) },

	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0) },
	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0) },

	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0) },
	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0) },

	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0) },
	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0) }
};

//*** Cube edges - ordered indices into the vertex array
static const int NumTri = 12;
static const int CubeTri[] = {
	0, 1, 2, 0, 2, 3,
	4, 5, 6, 4, 6, 7,
	8, 9, 10, 8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23
};

void HWD3DGame_DX3::Init(HWND TargetWnd)
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

	//Back Surface
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

	// WVP Matrices
	{
		m_D3DDevice->CreateMatrix(&m_MatrixProj);
		m_D3DDevice->CreateMatrix(&m_MatrixView);
		m_D3DDevice->CreateMatrix(&m_MatrixWorld);

		const hwd3d_matrix ProjMatrix = HWD3DMatrix_BuildPerspectiveFovLH(HWD3D_ToRad(90.f), (static_cast<float>(ScreenWidth)/ScreenHeight), .1f , 1000.f );

		D3DMATRIX Proj = *reinterpret_cast<const D3DMATRIX*>(&ProjMatrix);
		D3DMATRIX View = DHWG_View;
		D3DMATRIX World = DHWG_Ident;

		m_D3DDevice->SetMatrix(m_MatrixProj, &Proj);
		m_D3DDevice->SetMatrix(m_MatrixView, &View);
		m_D3DDevice->SetMatrix(m_MatrixWorld, &World);
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
	}

	// Mesh
	const bool bExecBufferCreated = CreateExecBuffer();
	if (!bExecBufferCreated)
	{
		Deinit();
		return;
	}
}

void HWD3DGame_DX3::Deinit()
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

	SafeRelease(m_ExecBuffer);

	SafeRelease(m_BgMaterial);
	m_BgMaterialHandle = 0;
	SafeRelease(m_Viewport);

	SafeDeleteMatrix(m_MatrixWorld);
	SafeDeleteMatrix(m_MatrixView);
	SafeDeleteMatrix(m_MatrixProj);

	SafeRelease(m_D3DDevice);
	SafeRelease(m_BackBuffer);
	SafeRelease(m_PrimarySurface);
	SafeRelease(m_D3D);
	SafeRelease(m_DDraw);
}

void HWD3DGame_DX3::Update(float DeltaTime)
{
	m_Frame++;
}

void HWD3DGame_DX3::Render()
{
	if (m_Viewport)
	{
		D3DVIEWPORT Vp = {};
		Vp.dwSize = sizeof(Vp);
		m_Viewport->GetViewport(&Vp);
		D3DRECT ClearRect = { 0, 0, static_cast<LONG>(Vp.dwWidth), static_cast<LONG>(Vp.dwHeight) };
		m_Viewport->SetBackground(m_BgMaterialHandle);
		m_Viewport->Clear(1, &ClearRect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);
	}

	if (m_D3DDevice)
	{
		if (SUCCEEDED(m_D3DDevice->BeginScene()))
		{
			// m_D3DDevice->SetMatrix( m_MatrixWorld , &World );

			const HRESULT ExecRes = m_D3DDevice->Execute(m_ExecBuffer, m_Viewport, D3DEXECUTE_CLIPPED);
			assert(SUCCEEDED(ExecRes));

			const HRESULT EndSceneRes = m_D3DDevice->EndScene();
			assert(SUCCEEDED(EndSceneRes));

			D3DEXECUTEDATA ExecData = {};
			ExecData.dwSize = sizeof(ExecData);
			const HRESULT GetExecDataRes = m_ExecBuffer->GetExecuteData(&ExecData);
			assert(SUCCEEDED(GetExecDataRes));
		}
	}

	if (m_DDraw && m_BackBuffer)
	{
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

			HRESULT Res = m_PrimarySurface->Blt(&RcWnd, m_BackBuffer, &rcSource, DDBLT_WAIT, 0);
			if (Res == DDERR_SURFACELOST)
			{
				//  Restore();
				return;
			}
		}
	}
}

bool HWD3DGame_DX3::CreateExecBuffer()
{
	m_ExecBufferDesc.dwSize = sizeof(m_ExecBufferDesc);
	m_ExecBufferDesc.dwFlags = D3DDEB_BUFSIZE;
	m_ExecBufferDesc.dwBufferSize = 1024 * 1024;
	const HRESULT CreateExecBufferRes = m_D3DDevice->CreateExecuteBuffer(&m_ExecBufferDesc, &m_ExecBuffer, nullptr);
	if (FAILED(CreateExecBufferRes) || !m_ExecBuffer)
	{
		return false;
	}

	if (SUCCEEDED(m_ExecBuffer->Lock(&m_ExecBufferDesc)))
	{
		memset(m_ExecBufferDesc.lpData, 0, m_ExecBufferDesc.dwBufferSize);

		LPVOID lpBufStart = m_ExecBufferDesc.lpData;
		LPVOID lpPointer = lpBufStart;

		VERTEX_DATA(&CubeVertices[0], NumVertices, lpPointer);

		LPVOID lpInsStart = lpPointer;

		OP_STATE_TRANSFORM(3, lpPointer);
		STATE_DATA(D3DTRANSFORMSTATE_PROJECTION, m_MatrixProj, lpPointer);
		STATE_DATA(D3DTRANSFORMSTATE_VIEW, m_MatrixView, lpPointer);
		STATE_DATA(D3DTRANSFORMSTATE_WORLD, m_MatrixWorld, lpPointer);

		OP_SET_STATUS(D3DSETSTATUS_ALL, D3DSTATUS_DEFAULT, 2048, 2048, 0, 0, lpPointer);

		OP_PROCESS_VERTICES(1, lpPointer);
		PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, 0, NumVertices, lpPointer);

		OP_STATE_RENDER(3, lpPointer);
		STATE_DATA(D3DRENDERSTATE_TEXTUREHANDLE, 0, lpPointer);
		STATE_DATA(D3DRENDERSTATE_WRAPU, FALSE, lpPointer);
		STATE_DATA(D3DRENDERSTATE_WRAPV, FALSE, lpPointer);

		// Make sure that the triangle data (not OP) will be QWORD aligned
		if (QWORD_ALIGNED(lpPointer))
		{
			OP_NOP(lpPointer);
		}

		OP_TRIANGLE_LIST(NumTri, lpPointer);
		for (int i = 0; i < NumTri; i++)
		{
			((LPD3DTRIANGLE)lpPointer)->v1 = CubeTri[i * 3];
			((LPD3DTRIANGLE)lpPointer)->v2 = CubeTri[i * 3 + 1];
			((LPD3DTRIANGLE)lpPointer)->v3 = CubeTri[i * 3 + 2];
			((LPD3DTRIANGLE)lpPointer)->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE;

			lpPointer = (void*)(((LPD3DTRIANGLE)lpPointer) + 1);
		}

		OP_EXIT(lpPointer);

		const HRESULT UnlockRes = m_ExecBuffer->Unlock();
		if (FAILED(UnlockRes))
		{
			return false;
		}

		D3DEXECUTEDATA ExecData = {};
		ExecData.dwSize = sizeof(ExecData);
		ExecData.dwInstructionOffset = (ULONG)((char*)lpInsStart - (char*)lpBufStart);
		ExecData.dwInstructionLength = (ULONG)((char*)lpPointer - (char*)lpInsStart);
		ExecData.dwVertexCount = NumVertices;
		const HRESULT SetDataRes = m_ExecBuffer->SetExecuteData(&ExecData);
		if (FAILED(SetDataRes))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

HRESULT FAR PASCAL HWD3DGame_DX3::D3DCb_EnumDevices(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC DevDesc1, LPD3DDEVICEDESC DevDesc2, LPVOID Context)
{
	HWD3DGame_DX3* _this = reinterpret_cast<HWD3DGame_DX3*>(Context);

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
