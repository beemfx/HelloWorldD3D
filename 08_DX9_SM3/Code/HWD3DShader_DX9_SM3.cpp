// D3D Hello World

#include "HWD3DShader_DX9_SM3.h"
#include "HWD3DGame_DX9_SM3.h"

HWD3DShader_DX9_SM3* HWD3DShader_DX9_SM3::CreateShader(class HWD3DGame* InGame, const char* InVSFile, const char* InPSFile)
{
	return new HWD3DShader_DX9_SM3(dynamic_cast<HWD3DGame_DX9_SM3*>(InGame), InVSFile, InPSFile);
}

void HWD3DShader_DX9_SM3::Release()
{
	delete this;
}

void HWD3DShader_DX9_SM3::SetShader()
{
	if (m_Game && m_Game->GetDevice() && m_VS && m_PS && m_VD)
	{
		m_Game->GetDevice()->SetVertexDeclaration(m_VD);
		m_Game->GetDevice()->SetVertexShader(m_VS);
		m_Game->GetDevice()->SetPixelShader(m_PS);
	}
}

HWD3DShader_DX9_SM3::HWD3DShader_DX9_SM3(class HWD3DGame_DX9_SM3* InGame, const char* InVSFile, const char* InPSFile)
	: m_Game(InGame)
{
	IDirect3DDevice9* Dev = m_Game ? m_Game->GetDevice() : nullptr;

	if (!Dev)
	{
		return;
	}

	// Vertex Declaration
	{
		static const D3DVERTEXELEMENT9 Vd[] =
		{
			{ 0 , 0 , D3DDECLTYPE_FLOAT3 , D3DDECLMETHOD_DEFAULT , D3DDECLUSAGE_POSITION , 0 } ,
			{ 0 , 12 , D3DDECLTYPE_FLOAT3 , D3DDECLMETHOD_DEFAULT , D3DDECLUSAGE_NORMAL , 0 } ,
			{ 0 , 24 , D3DDECLTYPE_FLOAT2 , D3DDECLMETHOD_DEFAULT , D3DDECLUSAGE_TEXCOORD , 0 } ,
			D3DDECL_END()
		};
		const HRESULT CreateVdRes = Dev->CreateVertexDeclaration(Vd, &m_VD);
		if (FAILED(CreateVdRes) || !m_VD)
		{
			return;
		}
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

		const HRESULT CreateRes = Dev->CreateVertexShader(reinterpret_cast<DWORD*>(FileData.data()), &m_VS);
		if (FAILED(CreateRes) || !m_VS)
		{
			return;
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

		const HRESULT CreateRes = Dev->CreatePixelShader(reinterpret_cast<DWORD*>(FileData.data()), &m_PS);
		if (FAILED(CreateRes) || !m_VS)
		{
			return;
		}
	}
}

HWD3DShader_DX9_SM3::~HWD3DShader_DX9_SM3()
{
	HWD3D_SafeRelease(m_PS);
	HWD3D_SafeRelease(m_VS);
}
