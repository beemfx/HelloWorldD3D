// D3D Hello World

#include "HWD3DTexture_DX9_SM3.h"
#include "HWD3DGame_DX9_SM3.h"

HWD3DTexture* HWD3DTexture::CreateTexture(class HWD3DGame* InGame, const char* InFilename)
{
	HWD3DTexture_DX9_SM3* Out = new HWD3DTexture_DX9_SM3(dynamic_cast<HWD3DGame_DX9_SM3*>(InGame));
	if (Out)
	{
		Out->LoadTextureFile(InFilename);
		Out->InitTexture();
	}

	return Out;
}

HWD3DTexture_DX9_SM3::HWD3DTexture_DX9_SM3(class HWD3DGame_DX9_SM3* InGame)
	: m_Game(InGame)
{

}

HWD3DTexture_DX9_SM3::~HWD3DTexture_DX9_SM3()
{
	HWD3D_SafeRelease(m_Texture);
}

void HWD3DTexture_DX9_SM3::InitTexture()
{
	IDirect3DDevice9* Dev = m_Game ? m_Game->GetDevice() : nullptr;

	if (!Dev)
	{
		return;
	}

	if (m_Width == 0 || m_Height == 0 || m_Pixels.size() != (m_Width*m_Height) )
	{
		return;
	}

	const HRESULT CreateSurfaceRes = Dev->CreateTexture(m_Width, m_Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_Texture, NULL);
	if (FAILED(CreateSurfaceRes) || !m_Texture)
	{
		return;
	}

	const int PixelByteSize = 4;

	D3DLOCKED_RECT LockedRect = { };
	const HRESULT LockSurfRes = m_Texture->LockRect(0, &LockedRect, NULL, 0);
	if (SUCCEEDED(LockSurfRes))
	{
		assert(LockedRect.pBits);

		// Copy pixels:
		for (int SrcY = 0; SrcY < m_Width; SrcY++)
		{
			for (int SrcX = 0; SrcX < m_Height; SrcX++)
			{
				const hwd3d_rgba& SrcClr = m_Pixels[SrcY*m_Width + SrcX];
				const D3DCOLOR DstClr = (SrcClr.B<<0) | (SrcClr.G<<8) | (SrcClr.R<<16) | (SrcClr.A<<24);

				BYTE* SurfAsByteArray = reinterpret_cast<BYTE*>(LockedRect.pBits);
				const int PixelStartOffset = (SrcY*LockedRect.Pitch) + SrcX*PixelByteSize;
				assert( (PixelStartOffset + PixelByteSize) <= static_cast<int>(LockedRect.Pitch * m_Height) );
				BYTE* DstP = &SurfAsByteArray[PixelStartOffset];
				memcpy(DstP, &DstClr, PixelByteSize);
			}
		}

		const HRESULT UnlockSurfRes = m_Texture->UnlockRect(NULL);
		assert(SUCCEEDED(UnlockSurfRes));
	}
}

void HWD3DTexture_DX9_SM3::SetTexture()
{
	if (m_Game && m_Game->GetDevice() && m_Texture)
	{
		m_Game->GetDevice()->SetTexture(0, m_Texture);
		m_Game->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_Game->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_Game->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_Game->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_Game->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		m_Game->GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_Game->GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}
}
