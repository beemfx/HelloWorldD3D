// D3D Hello World

#include "HWD3DTexture_DX12.h"
#include "HWD3DGame_DX12.h"
#include "HWD3DBufferTexture_DX12.h"

HWD3DTexture* HWD3DTexture::CreateTexture(class HWD3DGame* InGame, const char* InFilename)
{
	HWD3DTexture_DX12* Out = new HWD3DTexture_DX12(dynamic_cast<HWD3DGame_DX12*>(InGame));
	if (Out)
	{
		Out->LoadTextureFile(InFilename);
		Out->InitTexture();
	}

	return Out;
}

HWD3DTexture_DX12::HWD3DTexture_DX12(class HWD3DGame_DX12* InGame)
	: m_Game(InGame)
{

}

HWD3DTexture_DX12::~HWD3DTexture_DX12()
{
	HWD3D_SafeRelease(m_TextureBuffer);
}

void HWD3DTexture_DX12::InitTexture()
{
	ID3D12Device* Dev = m_Game ? m_Game->GetDevice() : nullptr;

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

		hwd3dTextureFormat Format;
		Format.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Format.Width = m_Width;
		Format.Height = m_Height;

		m_TextureBuffer = HWD3DBufferTexture_DX12::CreateTexture(*Dev, m_Game->GetBufferViewProvider(), Format);
		if (m_TextureBuffer)
		{
			m_TextureBuffer->SetBufferData(TexturePixels.data(), TexturePixels.size()*4);
		}
	}
}

void HWD3DTexture_DX12::SetTexture()
{
	if (m_Game && m_Game->GetCommandList() && m_TextureBuffer && m_TextureBuffer->GetGpuDescHandle().ptr != 0)
	{
		m_TextureBuffer->PrepareForDraw(*m_Game->GetCommandList());
		m_Game->GetCommandList()->SetGraphicsRootDescriptorTable(1, m_TextureBuffer->GetGpuDescHandle());
	}
}
