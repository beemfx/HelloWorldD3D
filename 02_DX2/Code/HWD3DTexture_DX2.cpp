// D3D Hello World

#include "HWD3DTexture_DX2.h"
#include "HWD3DGame_DX2.h"
#include "d3dmacs.h"

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
	{
		m_ExecBufferDesc.dwSize = sizeof(m_ExecBufferDesc);
		m_ExecBufferDesc.dwFlags = D3DDEB_BUFSIZE;
		static const int NUM_INSTR = 2;
		static const int NUM_RENDER_STATES = 3;
		m_ExecBufferDesc.dwBufferSize = sizeof(D3DINSTRUCTION)*NUM_INSTR + sizeof(D3DSTATE)*NUM_RENDER_STATES;
		const HRESULT CreateExecBufferRes = Dev->CreateExecuteBuffer(&m_ExecBufferDesc, &m_ExecBuffer, nullptr);
		if (FAILED(CreateExecBufferRes) || !m_ExecBuffer)
		{
			return;
		}

		if (SUCCEEDED(m_ExecBuffer->Lock(&m_ExecBufferDesc)))
		{
			memset(m_ExecBufferDesc.lpData, 0, m_ExecBufferDesc.dwBufferSize);

			LPVOID lpBufStart = m_ExecBufferDesc.lpData;
			LPVOID lpPointer = lpBufStart;
			LPVOID lpInsStart = lpPointer;

			OP_STATE_RENDER(NUM_RENDER_STATES, lpPointer);
			STATE_DATA(D3DRENDERSTATE_TEXTUREHANDLE, m_TextureHandle, lpPointer);
			STATE_DATA(D3DRENDERSTATE_WRAPU, FALSE, lpPointer);
			STATE_DATA(D3DRENDERSTATE_WRAPV, FALSE, lpPointer);

			OP_EXIT(lpPointer);

			const HRESULT UnlockRes = m_ExecBuffer->Unlock();
			if (FAILED(UnlockRes))
			{
				return;
			}

			D3DEXECUTEDATA ExecData = {};
			ExecData.dwSize = sizeof(ExecData);
			ExecData.dwInstructionOffset = (ULONG)((char*)lpInsStart - (char*)lpBufStart);
			ExecData.dwInstructionLength = (ULONG)((char*)lpPointer - (char*)lpInsStart);
			ExecData.dwVertexCount = 0;
			const HRESULT SetDataRes = m_ExecBuffer->SetExecuteData(&ExecData);
			if (FAILED(SetDataRes))
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
}

void HWD3DTexture_DX2::SetTexture()
{
	if (m_ExecBuffer && m_Game && m_Game->GetDevice() && m_Game->GetViewport())
	{
		const HRESULT ExecRes = m_Game->GetDevice()->Execute(m_ExecBuffer, m_Game->GetViewport(), D3DEXECUTE_CLIPPED);
		assert(SUCCEEDED(ExecRes));
	}
}
