// D3D Hello World

#include "HWD3DTexture_DX11.h"
#include "HWD3DGame_DX11.h"

HWD3DTexture* HWD3DTexture::CreateTexture(class HWD3DGame* InGame, const char* InFilename)
{
	HWD3DTexture_DX11* Out = new HWD3DTexture_DX11(dynamic_cast<HWD3DGame_DX11*>(InGame));
	if (Out)
	{
		Out->LoadTextureFile(InFilename);
		Out->InitTexture();
	}

	return Out;
}

HWD3DTexture_DX11::HWD3DTexture_DX11(class HWD3DGame_DX11* InGame)
	: m_Game(InGame)
{

}

HWD3DTexture_DX11::~HWD3DTexture_DX11()
{
	HWD3D_SafeRelease(m_View);
	HWD3D_SafeRelease(m_Texture);
}

void HWD3DTexture_DX11::InitTexture()
{
	ID3D11Device* Dev = m_Game ? m_Game->GetDevice() : nullptr;

	if (!Dev)
	{
		return;
	}

	if (m_Width == 0 || m_Height == 0 || m_Pixels.size() != (m_Width*m_Height) )
	{
		return;
	}

	// Create Texture:
	{
		std::vector<DWORD> TexturePixels;
		TexturePixels.reserve(m_Width*m_Height);
		// Copy pixels:
		for (int SrcY = 0; SrcY < m_Width; SrcY++)
		{
			for (int SrcX = 0; SrcX < m_Height; SrcX++)
			{
				const hwd3d_rgba& SrcClr = m_Pixels[SrcY*m_Width + SrcX];
				const DWORD DstClr = (SrcClr.A<<24) | (SrcClr.B<<16) | (SrcClr.G<<8) | (SrcClr.R<<0);
				TexturePixels.push_back(DstClr);
			}
		}

		D3D11_TEXTURE2D_DESC TxDesc = { };
		TxDesc.Width = m_Width;
		TxDesc.Height = m_Height;
		TxDesc.MipLevels = 1;
		TxDesc.ArraySize = 1;
		TxDesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		TxDesc.Usage = D3D11_USAGE_IMMUTABLE;
		TxDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TxDesc.SampleDesc.Quality = 0;
		TxDesc.SampleDesc.Count = 1;

		D3D11_SUBRESOURCE_DATA Data = { };
		Data.pSysMem = TexturePixels.data();
		Data.SysMemPitch = m_Width*sizeof(DWORD);
		Data.SysMemSlicePitch = m_Width * m_Height * sizeof(DWORD);

		const HRESULT Res = Dev->CreateTexture2D(&TxDesc, &Data, &m_Texture);
		if (FAILED(Res) || !m_Texture)
		{
			return;
		}
	}

	// Create View:
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC ViewDesc = { };
		ViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		ViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		ViewDesc.Texture2D.MipLevels = 1;
		ViewDesc.Texture2D.MostDetailedMip = 0;
		const HRESULT Res = Dev->CreateShaderResourceView(m_Texture, &ViewDesc, &m_View);
		if (FAILED(Res) || !m_View)
		{
			return;
		}
	}
}

void HWD3DTexture_DX11::SetTexture()
{
	if (m_Game && m_Game->GetContext() && m_Texture && m_View)
	{
		m_Game->GetContext()->PSSetShaderResources(0, 1 , &m_View);
	}
}
