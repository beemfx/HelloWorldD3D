// D3D Hello World

#include "HWD3DTexture_DX2.h"
#include "HWD3DGame_DX2.h"

HWD3DTexture* HWD3DTexture::CreateTexture(class HWD3DGame* InGame, const char* InFilename)
{
	HWD3DTexture_DX2* Out = new HWD3DTexture_DX2(dynamic_cast<HWD3DGame_DX2*>(InGame));
	if (Out)
	{
		Out->LoadTextureFile(InFilename);
		Out->InitTexture();
	}

	return Out;
}

HWD3DTexture_DX2::HWD3DTexture_DX2(class HWD3DGame_DX2* InGame)
	: m_Game(InGame)
{

}

HWD3DTexture_DX2::~HWD3DTexture_DX2()
{
	HWD3D_SafeRelease(m_Texture);
	HWD3D_SafeRelease(m_Surface);
}

void HWD3DTexture_DX2::InitTexture()
{
	IDirectDraw* DDraw = m_Game ? m_Game->GetDirectDraw() : nullptr;
	IDirect3DDevice* Dev = m_Game ? m_Game->GetDevice() : nullptr;

	if (!DDraw || !Dev)
	{
		return;
	}

	if (m_Width == 0 || m_Height == 0 || m_Pixels.size() != (m_Width*m_Height) )
	{
		return;
	}

	DDSURFACEDESC SurfDesc = { };
	SurfDesc.dwSize = sizeof(SurfDesc);
	SurfDesc.dwFlags = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
	SurfDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE|DDSCAPS_ALLOCONLOAD|m_Game->GetMemFlag();
	SurfDesc.dwWidth = m_Width;
	SurfDesc.dwHeight = m_Height;

	DDPIXELFORMAT& Pxf = SurfDesc.ddpfPixelFormat;
	Pxf.dwSize = sizeof(Pxf);
	Pxf.dwFlags = DDPF_RGB|DDPF_ALPHAPIXELS;
	Pxf.dwRGBBitCount = 32;
	Pxf.dwRGBAlphaBitMask = 0xFF << 24;
	Pxf.dwRBitMask = 0xFF << 16;
	Pxf.dwGBitMask = 0xFF << 8;
	Pxf.dwBBitMask = 0xFF << 0;

	const HRESULT CreateSurfaceRes = DDraw->CreateSurface(&SurfDesc, &m_Surface, nullptr);
	if (FAILED(CreateSurfaceRes) || !m_Surface)
	{
		return;
	}

	DDSURFACEDESC Sd = { };
	Sd.dwSize = sizeof(Sd);
	Sd.dwFlags =  DDSD_ALL;

	const int PixelByteSize = 4;

	const HRESULT LockSurfRes = m_Surface->Lock(NULL, &Sd, 0, NULL);
	if (SUCCEEDED(LockSurfRes))
	{
		assert(Sd.lpSurface);

		// Copy pixels:
		for (int SrcY = 0; SrcY < m_Width; SrcY++)
		{
			for (int SrcX = 0; SrcX < m_Height; SrcX++)
			{
				const hwd3d_rgba& SrcClr = m_Pixels[SrcY*m_Width + SrcX];
				const D3DCOLOR DstClr = RGBA_SETALPHA(RGB_MAKE(SrcClr.R, SrcClr.G, SrcClr.B), SrcClr.A);

				BYTE* SurfAsByteArray = reinterpret_cast<BYTE*>(Sd.lpSurface);
				const int PixelStartOffset = (SrcY*Sd.lPitch) + SrcX*PixelByteSize;
				assert( (PixelStartOffset + PixelByteSize) <= static_cast<int>(Sd.lPitch * Sd.dwHeight) );
				BYTE* DstP = &SurfAsByteArray[PixelStartOffset];
				memcpy(DstP, &DstClr, PixelByteSize);
			}
		}

		const HRESULT UnlockSurfRes = m_Surface->Unlock(NULL);
		assert(SUCCEEDED(UnlockSurfRes));
	}

	const HRESULT QueryTextureRes = m_Surface->QueryInterface(IID_IDirect3DTexture, reinterpret_cast<LPVOID*>(&m_Texture));
	if (FAILED(QueryTextureRes) || !m_Texture)
	{
		return;
	}

	const HRESULT GetHandleRes = m_Texture->GetHandle(Dev, &m_TextureHandle);
	if (FAILED(GetHandleRes) || m_TextureHandle == 0)
	{
		return;
	}

	// Create Exec Buffer
	if (m_ExecBuffer = new HWD3DExecBuffer_DX2(m_Game))
	{		
		m_ExecBuffer->BeginInstructions();
		m_ExecBuffer->BeginRenderStates(5);
		m_ExecBuffer->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, m_TextureHandle);
		m_ExecBuffer->SetRenderState(D3DRENDERSTATE_WRAPU, FALSE);
		m_ExecBuffer->SetRenderState(D3DRENDERSTATE_WRAPV, FALSE);
		m_ExecBuffer->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
		m_ExecBuffer->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_LINEAR);
		m_ExecBuffer->FinalizeBuffer();
	}
}

void HWD3DTexture_DX2::SetTexture()
{
	if (m_ExecBuffer)
	{
		m_ExecBuffer->ExecuteBuffer();
	}
}
