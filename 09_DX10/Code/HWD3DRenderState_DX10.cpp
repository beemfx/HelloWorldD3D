// D3D Hello World

#include "HWD3DRenderState_DX10.h"
#include "HWD3DGame_DX10.h"

HWD3DRenderState_DX10* HWD3DRenderState_DX10::CreateRenderState(class HWD3DGame* InGame, const char* InVSFile, const char* InPSFile)
{
	return new HWD3DRenderState_DX10(dynamic_cast<HWD3DGame_DX10*>(InGame), InVSFile, InPSFile);
}

void HWD3DRenderState_DX10::Release()
{
	delete this;
}

void HWD3DRenderState_DX10::ApplyRenderState()
{
	if (m_Game && m_Game->GetDevice() && m_VS && m_IL)
	{
		m_Game->GetDevice()->IASetInputLayout(m_IL);
		m_Game->GetDevice()->VSSetShader(m_VS);
		m_Game->GetDevice()->PSSetShader(m_PS);
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
}

HWD3DRenderState_DX10::~HWD3DRenderState_DX10()
{
	HWD3D_SafeRelease(m_PS);
	HWD3D_SafeRelease(m_VS);
	HWD3D_SafeRelease(m_IL);
}
