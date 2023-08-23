// D3D Hello World

#include "HWD3DRenderState_DX12.h"
#include "HWD3DGame_DX12.h"

HWD3DRenderState_DX12* HWD3DRenderState_DX12::CreateRenderState(class HWD3DGame* InGame, const char* InVSFile, const char* InPSFile)
{
	return new HWD3DRenderState_DX12(dynamic_cast<HWD3DGame_DX12*>(InGame), InVSFile, InPSFile);
}

void HWD3DRenderState_DX12::Release()
{
	delete this;
}

void HWD3DRenderState_DX12::ApplyRenderState()
{
	ID3D12GraphicsCommandList* Cl = m_Game ? m_Game->GetCommandList() : nullptr;
	if (Cl && m_State)
	{
		Cl->SetPipelineState(m_State);
	}
}

HWD3DRenderState_DX12::HWD3DRenderState_DX12(class HWD3DGame_DX12* InGame, const char* InVSFile, const char* InPSFile)
	: m_Game(InGame)
{
	ID3D12Device* Dev = m_Game ? m_Game->GetDevice() : nullptr;
	ID3D12RootSignature* RootSig = m_Game ? m_Game->GetRootSig() : nullptr;

	if (!Dev || !RootSig)
	{
		return;
	}

	std::vector<BYTE> VSData;
	std::vector<BYTE> PSData;

	// Load Vertex Shader:
	{
		HANDLE ShaderFile = CreateFileA(InVSFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (ShaderFile == INVALID_HANDLE_VALUE)
		{
			return;
		}

		const DWORD FileSize = GetFileSize(ShaderFile, NULL);
		VSData.resize(FileSize);
		DWORD NumRead = 0;
		BOOL bRead = ReadFile(ShaderFile, VSData.data(), FileSize, &NumRead, NULL);
		CloseHandle(ShaderFile);
		if (!bRead || NumRead != FileSize)
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
		PSData.resize(FileSize);
		DWORD NumRead = 0;
		BOOL bRead = ReadFile(ShaderFile, PSData.data(), FileSize, &NumRead, NULL);
		CloseHandle(ShaderFile);
		if (!bRead || NumRead != FileSize)
		{
			return;
		}
	}

	// Vertex Declaration
	static const D3D12_INPUT_ELEMENT_DESC Vd[] =
	{
		{ "SV_POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0 , D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 },
		{ "NORMAL"      , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0 , D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD"    , 0 , DXGI_FORMAT_R32G32_FLOAT    , 0 , D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 },
	};

	D3D12_RASTERIZER_DESC Rsd = { };
	Rsd.FillMode = D3D12_FILL_MODE_SOLID;
	Rsd.CullMode = D3D12_CULL_MODE_BACK;
	Rsd.FrontCounterClockwise = FALSE;
	Rsd.DepthBias = 0;
	Rsd.DepthBiasClamp = 0.f;
	Rsd.SlopeScaledDepthBias = 0.f;
	Rsd.DepthClipEnable = TRUE;
	Rsd.MultisampleEnable = FALSE;
	Rsd.ForcedSampleCount = 0;
	Rsd.MultisampleEnable = FALSE;
	Rsd.AntialiasedLineEnable = FALSE;
	Rsd.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_BLEND_DESC Bd = { };
	Bd.AlphaToCoverageEnable = FALSE;
	Bd.IndependentBlendEnable = FALSE;
	for (auto& Rt : Bd.RenderTarget)
	{
		Rt.BlendEnable = TRUE;
		Rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		Rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		Rt.BlendOp = D3D12_BLEND_OP_ADD;
		Rt.SrcBlendAlpha = D3D12_BLEND_ONE;
		Rt.DestBlendAlpha = D3D12_BLEND_ZERO;
		Rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		Rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	const D3D12_DEPTH_STENCIL_DESC Dsd = 
		{ TRUE 
		, D3D12_DEPTH_WRITE_MASK_ALL 
		, D3D12_COMPARISON_FUNC_LESS
		, FALSE
		, D3D12_DEFAULT_STENCIL_READ_MASK
		, D3D12_DEFAULT_STENCIL_WRITE_MASK
		, { D3D12_STENCIL_OP_KEEP , D3D12_STENCIL_OP_KEEP , D3D12_STENCIL_OP_KEEP , D3D12_COMPARISON_FUNC_ALWAYS }
		, { D3D12_STENCIL_OP_KEEP , D3D12_STENCIL_OP_KEEP , D3D12_STENCIL_OP_KEEP , D3D12_COMPARISON_FUNC_ALWAYS } };

	DXGI_SAMPLE_DESC SampleDesc = { 1 , 0 };

	D3D12_STREAM_OUTPUT_DESC Od = { };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GpsDesc = { };
	GpsDesc.pRootSignature = RootSig;
	GpsDesc.VS.pShaderBytecode = VSData.data();
	GpsDesc.VS.BytecodeLength = VSData.size();
	GpsDesc.PS.pShaderBytecode = PSData.data();
	GpsDesc.PS.BytecodeLength = PSData.size();
	GpsDesc.StreamOutput = Od;
	GpsDesc.InputLayout.pInputElementDescs = Vd;
	GpsDesc.InputLayout.NumElements = _countof(Vd);
	GpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	GpsDesc.RasterizerState = Rsd;
	GpsDesc.SampleDesc = SampleDesc;
	GpsDesc.BlendState = Bd;
	GpsDesc.DepthStencilState = Dsd;
	GpsDesc.NumRenderTargets = 1;
	GpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	GpsDesc.DSVFormat = DXGI_FORMAT_D16_UNORM;
	GpsDesc.SampleMask = 0xFFFFFFFF;
	GpsDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	GpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	const HRESULT CreateStateRes = Dev->CreateGraphicsPipelineState(&GpsDesc, IID_PPV_ARGS(&m_State));
	if (FAILED(CreateStateRes) || !m_State)
	{
		assert(false); // Bad pipeline state?
		return;
	}
}

HWD3DRenderState_DX12::~HWD3DRenderState_DX12()
{
	HWD3D_SafeRelease(m_State);
}
