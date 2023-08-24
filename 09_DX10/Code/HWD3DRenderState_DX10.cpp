// D3D Hello World

#include "HWD3DRenderState_DX10.h"
#include "HWD3DGame_DX10.h"

HWD3DRenderState_DX10* HWD3DRenderState_DX10::CreateRenderState(class HWD3DGame* InGame, const char* InVSFile, const char* InPSFile)
{
	return new HWD3DRenderState_DX10(dynamic_cast<HWD3DGame_DX10*>(InGame), InVSFile, InPSFile);
}

void HWD3DRenderState_DX10::ApplyRenderState()
{
	ID3D10Device* Dev = m_Game ? m_Game->GetDevice() : nullptr;
	if (Dev && m_VS && m_IL && m_RS && m_SS && m_BS && m_DS)
	{
		Dev->IASetInputLayout(m_IL);
		Dev->VSSetShader(m_VS);
		Dev->PSSetShader(m_PS);
		Dev->RSSetState(m_RS);
		Dev->PSSetSamplers(0, 1, &m_SS);
		const float BlendFactor[] = { 1.f , 1.f , 1.f , 1.f };
		Dev->OMSetBlendState(m_BS, BlendFactor, 0xFFFFFFFF);
		Dev->OMSetDepthStencilState(m_DS, 0);
	}
}

HWD3DRenderState_DX10::HWD3DRenderState_DX10(class HWD3DGame_DX10* InGame, const char* InVSFile, const char* InPSFile)
	: m_Game(InGame)
{
	ID3D10Device* Dev = m_Game ? m_Game->GetDevice() : nullptr;

	if (!Dev)
	{
		return;
	}

	// Load Vertex Shader:
	{
		HANDLE ShaderFile = CreateFileA(InVSFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (ShaderFile == INVALID_HANDLE_VALUE)
		{
			return;
		}

		const DWORD FileSize = GetFileSize(ShaderFile, NULL);
		std::vector<BYTE> FileData;
		FileData.resize(FileSize);
		DWORD NumRead = 0;
		BOOL bRead = ReadFile(ShaderFile, FileData.data(), FileSize, &NumRead, NULL);
		CloseHandle(ShaderFile);
		if (!bRead || NumRead != FileSize)
		{
			return;
		}

		const HRESULT CreateRes = Dev->CreateVertexShader(reinterpret_cast<DWORD*>(FileData.data()), FileData.size(), &m_VS);
		if (FAILED(CreateRes) || !m_VS)
		{
			return;
		}

		// Vertex Declaration
		{
			static const D3D10_INPUT_ELEMENT_DESC Vd[] =
			{
				{ "SV_POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0 , D3D10_APPEND_ALIGNED_ELEMENT , D3D10_INPUT_PER_VERTEX_DATA , 0 },
				{ "NORMAL"      , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0 , D3D10_APPEND_ALIGNED_ELEMENT , D3D10_INPUT_PER_VERTEX_DATA , 0 },
				{ "TEXCOORD"    , 0 , DXGI_FORMAT_R32G32_FLOAT    , 0 , D3D10_APPEND_ALIGNED_ELEMENT , D3D10_INPUT_PER_VERTEX_DATA , 0 },
			};
			const HRESULT CreateVdRes = Dev->CreateInputLayout(Vd, _countof(Vd), reinterpret_cast<const void*>(FileData.data()), FileData.size(), &m_IL);
			if (FAILED(CreateVdRes) || !m_IL)
			{
				return;
			}
		}
	}

	// Load PixelShader
	{
		HANDLE ShaderFile = CreateFileA(InPSFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (ShaderFile == INVALID_HANDLE_VALUE)
		{
			return;
		}

		const DWORD FileSize = GetFileSize(ShaderFile, NULL);
		std::vector<BYTE> FileData;
		FileData.resize(FileSize);
		DWORD NumRead = 0;
		BOOL bRead = ReadFile(ShaderFile, FileData.data(), FileSize, &NumRead, NULL);
		CloseHandle(ShaderFile);
		if (!bRead || NumRead != FileSize)
		{
			return;
		}

		const HRESULT CreateRes = Dev->CreatePixelShader(reinterpret_cast<DWORD*>(FileData.data()), FileData.size(), &m_PS);
		if (FAILED(CreateRes) || !m_VS)
		{
			return;
		}
	}

	// Raster State:
	{
		D3D10_RASTERIZER_DESC Rsd = { };
		Rsd.FillMode = D3D10_FILL_SOLID;
		Rsd.CullMode = D3D10_CULL_BACK;
		Rsd.FrontCounterClockwise = FALSE;
		Rsd.DepthBias = 0;
		Rsd.DepthBiasClamp = 0.f;
		Rsd.SlopeScaledDepthBias = 0.f;
		Rsd.DepthClipEnable = TRUE;
		Rsd.ScissorEnable = FALSE;
		Rsd.MultisampleEnable = FALSE;
		Rsd.AntialiasedLineEnable = FALSE;
		const HRESULT Res = Dev->CreateRasterizerState(&Rsd, &m_RS);
		if (FAILED(Res) || !m_RS)
		{
			return;
		}
	}

	// Sampler State:
	{
		D3D10_SAMPLER_DESC Ssd = { };
		Ssd.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		Ssd.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		Ssd.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		Ssd.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
		Ssd.MipLODBias = 0.f;
		Ssd.MaxAnisotropy = 16;
		Ssd.ComparisonFunc = D3D10_COMPARISON_NEVER;
		Ssd.BorderColor[0] = 0.f;
		Ssd.BorderColor[1] = 0.f;
		Ssd.BorderColor[2] = 0.f;
		Ssd.BorderColor[3] = 0.f;
		Ssd.MinLOD = 0.f;
		Ssd.MaxLOD = FLT_MAX;

		const HRESULT Res = Dev->CreateSamplerState(&Ssd, &m_SS);
		if (FAILED(Res) || !m_SS)
		{
			return;
		}
	}

	// Blend State:
	{
		D3D10_BLEND_DESC Bd = { };
		Bd.AlphaToCoverageEnable = FALSE;
		Bd.BlendEnable[0] = TRUE;
		Bd.SrcBlend = D3D10_BLEND_SRC_ALPHA;
		Bd.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
		Bd.BlendOp = D3D10_BLEND_OP_ADD;
		Bd.SrcBlendAlpha = D3D10_BLEND_ONE;
		Bd.DestBlendAlpha = D3D10_BLEND_ZERO;
		Bd.BlendOpAlpha = D3D10_BLEND_OP_ADD;
		for (auto& Value : Bd.RenderTargetWriteMask)
		{
			Value = D3D10_COLOR_WRITE_ENABLE_ALL;
		}

		const HRESULT Res = Dev->CreateBlendState(&Bd, &m_BS);
		if (FAILED(Res) || !m_BS)
		{
			return;
		}
	}

	// Depth Stencil State:
	{
		const D3D10_DEPTH_STENCIL_DESC Dsd = 
		{ TRUE 
			, D3D10_DEPTH_WRITE_MASK_ALL 
			, D3D10_COMPARISON_LESS
			, FALSE
			, D3D10_DEFAULT_STENCIL_READ_MASK
			, D3D10_DEFAULT_STENCIL_WRITE_MASK
			, { D3D10_STENCIL_OP_KEEP , D3D10_STENCIL_OP_KEEP , D3D10_STENCIL_OP_KEEP , D3D10_COMPARISON_ALWAYS }
		, { D3D10_STENCIL_OP_KEEP , D3D10_STENCIL_OP_KEEP , D3D10_STENCIL_OP_KEEP , D3D10_COMPARISON_ALWAYS } };

		const HRESULT Res = Dev->CreateDepthStencilState(&Dsd, &m_DS);
		if (FAILED(Res) || !m_DS)
		{
			return;
		}
	}
}

HWD3DRenderState_DX10::~HWD3DRenderState_DX10()
{
	HWD3D_SafeRelease(m_PS);
	HWD3D_SafeRelease(m_VS);
	HWD3D_SafeRelease(m_IL);
	HWD3D_SafeRelease(m_SS);
	HWD3D_SafeRelease(m_RS);
	HWD3D_SafeRelease(m_DS);
	HWD3D_SafeRelease(m_BS);
}
