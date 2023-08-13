// D3D Hello World

#include "HWD3DGame_DX3.h"
#include "d3dmacs.h"

// #pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	HWD3DGame* Out = new HWD3DGame_DX3;
	if( Out )
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

static const D3DMATRIX DHWG_Proj_Ortho =
{
	D3DVAL(2.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(2.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0/(100.f - -100.f)), D3DVAL(0.f),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-100.f/(-100.f - 100.f)), D3DVAL(1.f)
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
	{D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0) },
	{D3DVAL(1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0) },
	{D3DVAL(1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0) },
	{D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0) },

	{D3DVAL(1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0) },
	{D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(1.0) },
	{D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(1.0) },
	{D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0) },

	{D3DVAL(1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0) },
	{D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0) },
	{D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0) },
	{D3DVAL(1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0) },

	{D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0) },
	{D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(1.0) },
	{D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(1.0) },
	{D3DVAL(1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(0.0) },

	{D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0) },
	{D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0) },
	{D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0) },
	{D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0) },

	{D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(1.0) },
	{D3DVAL(1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(1.0) },
	{D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(1.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(0.0),D3DVAL(0.0) },
	{D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(-1.0),D3DVAL(0.0),D3DVAL(1.0),D3DVAL(0.0) }
};

//*** Cube edges - ordered indices into the vertex array
static const int NumTri = 12;
static const int CubeTri[] = {
	0,  1,  2,  0, 2, 3,
	4,  5,  6,  4, 6, 7,
	8,  9, 10, 8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23
};

void HWD3DGame_DX3::Init(HWND TargetWnd)
{
	m_TargetWnd = TargetWnd;

	RECT TargetWndRc = { };
	GetClientRect (m_TargetWnd, &TargetWndRc);
	const int ScreenWidth = TargetWndRc.right - TargetWndRc.left;
	const int ScreenHeight = TargetWndRc.bottom - TargetWndRc.top;

	HRESULT CoInitRes = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	// CoCreateInstance(CLSID_Direct3D, NULL, CLSCTX_ALL, IID_IDirect3D, reinterpret_cast<LPVOID*>(&D3D)); 
	HRESULT CciRes = CoCreateInstance(CLSID_DirectDraw, NULL, CLSCTX_ALL, IID_IDirectDraw, reinterpret_cast<LPVOID*>(&m_DDraw));

	if( m_DDraw )
	{
		HRESULT InitRes = m_DDraw->Initialize(nullptr);
		HRESULT SetCpLvlRes = m_DDraw->SetCooperativeLevel(TargetWnd, DDSCL_NORMAL);
		
		m_DDraw->QueryInterface(IID_IDirect3D, reinterpret_cast<LPVOID*>(&m_D3D));
		m_D3D->EnumDevices(&D3DCb_EnumDevices, this);

		{
			DDSURFACEDESC BbSd = { };
			BbSd.dwSize = sizeof(BbSd);
			BbSd.dwFlags = DDSD_CAPS;
			BbSd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			HRESULT CreateBbRes = m_DDraw->CreateSurface(&BbSd, &m_PrimarySurface, nullptr);
			CreateBbRes = CreateBbRes;
		}

		{
			IDirectDrawClipper* Clipper = 0;
			m_DDraw->CreateClipper( 0 , &Clipper , 0 );
			Clipper->SetHWnd( 0 , TargetWnd );
			m_PrimarySurface->SetClipper( Clipper );
			Clipper->Release();
			Clipper = 0;
		}

		//Back Surface
		{
			DDSURFACEDESC BbSd = { };
			BbSd.dwSize = sizeof(BbSd);
			BbSd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
			BbSd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_3DDEVICE|DDSCAPS_VIDEOMEMORY;
			BbSd.dwWidth = ScreenWidth;
			BbSd.dwHeight = ScreenHeight;
			HRESULT CreateBbRes = m_DDraw->CreateSurface( &BbSd , &m_BackBuffer , 0 );
			assert(SUCCEEDED(CreateBbRes));
		}
		// HRESULT Res = m_DDraw->SetDisplayMode(640, 480, 32);
		// Res = Res;

		// m_D3D->QueryInterface(IID_IDirect3DDevice, reinterpret_cast<LPVOID*>(&m_D3DDevice));

		D3DFINDDEVICERESULT FindDev = { };
		FindDev.dwSize = sizeof(FindDev);
		D3DFINDDEVICESEARCH SearchDev = { };
		SearchDev.dwSize = sizeof(SearchDev);
		SearchDev.guid = m_DevicesFound.back().Id;
		m_D3D->FindDevice(&SearchDev, &FindDev);

		// HRESULT D3DInitRes = m_D3D->Initialize(IID_IDirect3D);// FindDev.guid);
		// assert( D3DInitRes == DDERR_ALREADYINITIALIZED );
		// CoCreateInstance(FindDev.guid, NULL, CLSCTX_ALL, IID_IDirect3DDevice, reinterpret_cast<LPVOID*>(&m_D3DDevice));
		HRESULT QueryForDevRes = m_BackBuffer->QueryInterface(FindDev.guid, reinterpret_cast<LPVOID*>(&m_D3DDevice));
		assert(SUCCEEDED(QueryForDevRes) && m_D3DDevice);

		m_D3DDevice->CreateMatrix( &m_MatrixProj );
		m_D3DDevice->CreateMatrix( &m_MatrixView );
		m_D3DDevice->CreateMatrix( &m_MatrixWorld );

		D3DMATRIX Proj = DHWG_Proj;
		D3DMATRIX View = DHWG_View;
		D3DMATRIX World = DHWG_Ident;

		m_D3DDevice->SetMatrix( m_MatrixProj , &Proj );
		m_D3DDevice->SetMatrix( m_MatrixView , &View );
		m_D3DDevice->SetMatrix( m_MatrixWorld , &World );

		m_D3D->CreateViewport( &m_Viewport , nullptr );
		m_D3DDevice->AddViewport( m_Viewport );
		D3DVIEWPORT Vp = { };
		Vp.dwSize = sizeof(Vp);
		Vp.dwX = Vp.dwY = 0;
		Vp.dwWidth = ScreenWidth;
		Vp.dwHeight = ScreenHeight;
		Vp.dvScaleX = Vp.dwWidth / 2.f;
		Vp.dvScaleY = Vp.dwHeight / 2.f;
		Vp.dvMaxX = D3DVAL(Vp.dwWidth) / D3DVAL(2.f * Vp.dvScaleX);
		Vp.dvMaxY = D3DVAL(Vp.dwHeight) / D3DVAL(2.f * Vp.dvScaleY);
		Vp.dvMaxZ = 1.f;
		m_Viewport->SetViewport( &Vp );

		{
			m_D3D->CreateMaterial( &m_Material , nullptr );
			D3DMATERIAL MaterialDesc = { };
			MaterialDesc.dwSize = sizeof(MaterialDesc);
			MaterialDesc.diffuse.r = 1.f;
			MaterialDesc.diffuse.g = 1.f;
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
			MaterialDesc.power = (float)20.0;
			MaterialDesc.dwRampSize = 16;
			// MaterialDesc.hTexture = TextureHandle[1];
			m_Material->SetMaterial(&MaterialDesc);
			m_Material->GetHandle(m_D3DDevice, &m_MaterialHandle);
		}		
		
		{
			m_D3D->CreateMaterial( &m_BgMaterial , nullptr );
			D3DMATERIAL MaterialDesc = { };
			MaterialDesc.dwSize = sizeof(MaterialDesc);
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
			MaterialDesc.power = (float)20.0;
			MaterialDesc.dwRampSize = 16;
			// MaterialDesc.hTexture = TextureHandle[1];
			m_Material->SetMaterial(&MaterialDesc);
			m_Material->GetHandle(m_D3DDevice, &m_BgMaterialHandle);
		}

		CreateExecBuffer();
	}
}

void HWD3DGame_DX3::Deinit()
{
	if( m_BackBuffer )
	{
		m_BackBuffer->Release();
		m_BackBuffer = nullptr;
	}

	if( m_PrimarySurface )
	{
		m_PrimarySurface->Release();
		m_PrimarySurface = nullptr;
	}

	if (m_D3DDevice)
	{
		ULONG NumLeft = m_D3DDevice->Release();
		m_D3DDevice = nullptr;
	}

	if (m_D3D)
	{
		ULONG NumLeft = m_D3D->Release();
		m_D3D = nullptr;
	}

	if (m_DDraw)
	{
		ULONG NumLeft = m_DDraw->Release();
		m_DDraw = nullptr;
	}
}

void HWD3DGame_DX3::Update(float DeltaTime)
{
	m_Frame++;
}

void HWD3DGame_DX3::Render()
{
	if( m_Viewport )
	{
		D3DVIEWPORT Vp = { };
		Vp.dwSize = sizeof(Vp);
		m_Viewport->GetViewport( &Vp );
		D3DRECT ClearRect = { 0 , 0 , static_cast<LONG>(Vp.dwWidth) , static_cast<LONG>(Vp.dwHeight) };
		m_Viewport->SetBackground( m_BgMaterialHandle );
		m_Viewport->Clear( 1 , &ClearRect , D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER );
	}

	if (m_D3DDevice)
	{
		if( SUCCEEDED(m_D3DDevice->BeginScene()) )
		{
			// D3DMATRIX& Proj = const_cast<D3DMATRIX&>(DHWG_Proj);
			// D3DMATRIX& View = const_cast<D3DMATRIX&>(DHWG_View);
			// D3DMATRIX& World = const_cast<D3DMATRIX&>(DHWG_Ident);
			// 
			// m_D3DDevice->SetMatrix( m_MatrixProj , &Proj );
			// m_D3DDevice->SetMatrix( m_MatrixView , &View );
			// m_D3DDevice->SetMatrix( m_MatrixWorld , &World );

			const HRESULT ExecRes = m_D3DDevice->Execute(m_ExecBuffer, m_Viewport, D3DEXECUTE_CLIPPED );
			assert(SUCCEEDED(ExecRes));

			const HRESULT EndSceneRes = m_D3DDevice->EndScene();
			assert(SUCCEEDED(EndSceneRes));

			D3DEXECUTEDATA ExecData = { };
			ExecData.dwSize = sizeof(ExecData);
			const HRESULT GetExecDataRes = m_ExecBuffer->GetExecuteData(&ExecData);
			assert(SUCCEEDED(GetExecDataRes));
		}
	}

	if (m_DDraw && m_BackBuffer)
	{
		{
			DDSURFACEDESC desc = { };
			desc.dwSize = sizeof(desc);
			desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT;
			m_BackBuffer->GetSurfaceDesc(&desc);

			RECT rcSource = { };
			rcSource.top = rcSource.left = 0;
			rcSource.bottom = desc.dwHeight;
			rcSource.right = desc.dwWidth;

			RECT RcWnd = { };
			GetClientRect( m_TargetWnd , &RcWnd );
			POINT Offset = { };
			ClientToScreen( m_TargetWnd , &Offset );
			OffsetRect( &RcWnd , Offset.x , Offset.y );
			
			HRESULT Res = m_PrimarySurface->Blt(&RcWnd, m_BackBuffer, &rcSource, DDBLT_WAIT, 0);
			if (Res == DDERR_SURFACELOST)
			{
				//  Restore();
				return;
			}
		}

		// HRESULT FlipRes = m_DDraw->FlipToGDISurface();
		// FlipRes = FlipRes;
	}
}

void HWD3DGame_DX3::CreateExecBuffer()
{
	m_ExecBufferDesc.dwSize = sizeof(m_ExecBufferDesc);
	m_ExecBufferDesc.dwFlags = D3DDEB_BUFSIZE;
	m_ExecBufferDesc.dwBufferSize = 1024*1024;
	const HRESULT CreateExecBufferRes = m_D3DDevice->CreateExecuteBuffer( &m_ExecBufferDesc , &m_ExecBuffer , nullptr );
	assert(SUCCEEDED(CreateExecBufferRes));

	if( m_ExecBuffer )
	{
		if( SUCCEEDED(m_ExecBuffer->Lock( &m_ExecBufferDesc )) )
		{
			LPVOID lpBufStart = m_ExecBufferDesc.lpData;

			memset( lpBufStart , 0 , m_ExecBufferDesc.dwBufferSize );
			LPVOID lpPointer = lpBufStart;

			VERTEX_DATA(&CubeVertices[0], NumVertices, lpPointer);

			LPVOID lpInsStart = lpPointer;

			OP_STATE_TRANSFORM(3, lpPointer);
			STATE_DATA(D3DTRANSFORMSTATE_PROJECTION, m_MatrixProj, lpPointer);
			STATE_DATA(D3DTRANSFORMSTATE_VIEW, m_MatrixView, lpPointer);
			STATE_DATA(D3DTRANSFORMSTATE_WORLD, m_MatrixWorld, lpPointer);

			OP_SET_STATUS(D3DSETSTATUS_ALL, D3DSTATUS_DEFAULT, 2048, 2048, 0, 0, lpPointer);

			// OP_PROCESS_VERTICES(1, lpPointer);
			// PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, 0, NumVertices, lpPointer);

			for (int i = 0; i < 6; i++) {
				OP_PROCESS_VERTICES(1, lpPointer);
				PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, i * 4, 4, lpPointer);
			}

			OP_STATE_RENDER(3, lpPointer);
			STATE_DATA(D3DRENDERSTATE_TEXTUREHANDLE, 0, lpPointer);
			STATE_DATA(D3DRENDERSTATE_WRAPU, FALSE, lpPointer);
			STATE_DATA(D3DRENDERSTATE_WRAPV, FALSE, lpPointer);
			/*
			* Make sure that the triangle data (not OP) will be QWORD aligned
			*/
			if (QWORD_ALIGNED(lpPointer)) {
				OP_NOP(lpPointer);
			}
			OP_TRIANGLE_LIST(NumTri, lpPointer);
			for (int i = 0; i < NumTri; i++)
			{
				((LPD3DTRIANGLE)lpPointer)->v1 = CubeTri[i*3];
				((LPD3DTRIANGLE)lpPointer)->v2 = CubeTri[i*3 + 1];
				((LPD3DTRIANGLE)lpPointer)->v3 = CubeTri[i*3 + 2];
				((LPD3DTRIANGLE)lpPointer)->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE;

				lpPointer = (void *)(((LPD3DTRIANGLE) lpPointer) + 1);
			}

			OP_EXIT(lpPointer);

			m_ExecBuffer->Unlock();
			D3DEXECUTEDATA ExecData = { };
			ExecData.dwSize = sizeof(ExecData);
			ExecData.dwInstructionOffset = (ULONG)((char*)lpInsStart - (char*)lpBufStart);
			ExecData.dwInstructionLength = (ULONG)((char*)lpPointer - (char*)lpInsStart);
			ExecData.dwVertexCount = NumVertices;
			const HRESULT SetDataRes = m_ExecBuffer->SetExecuteData(&ExecData);
			assert(SUCCEEDED(SetDataRes));
		}
	}
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

	const DWORD WantedCaps = D3DDEVCAPS_EXECUTEVIDEOMEMORY|D3DDEVCAPS_TLVERTEXVIDEOMEMORY|D3DDEVCAPS_TEXTUREVIDEOMEMORY;

	if( (d1.dwDevCaps&WantedCaps) == WantedCaps )
	{
		return S_OK;
	}

	return S_FALSE;
}
