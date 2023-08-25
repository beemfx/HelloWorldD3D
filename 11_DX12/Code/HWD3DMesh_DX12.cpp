// D3D Hello World

#include "HWD3DMesh_DX12.h"
#include "HWD3DGame_DX12.h"
#include "HWD3DBufferConstant_DX12.h"

HWD3DMesh* HWD3DMesh::CreateMesh(class HWD3DGame* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX12(dynamic_cast<HWD3DGame_DX12*>(InGame), InFilename);
}

void HWD3DMesh_DX12::Draw()
{
	if (m_Game && m_Game->GetCommandList() && m_VBBuffer && m_IBBuffer)
	{
		m_Game->PreDraw();
		m_VBBuffer->PrepareForDraw(*m_Game->GetCommandList());
		m_IBBuffer->PrepareForDraw(*m_Game->GetCommandList());

		const D3D12_VERTEX_BUFFER_VIEW VbViews[] = { { m_VBBuffer->GetGpuVirtualAddress() , m_VBBuffer->GetBufferSize() , sizeof(hwd3d_vertex) }, };
		m_Game->GetCommandList()->IASetVertexBuffers(0, _countof(VbViews), VbViews);
		const D3D12_INDEX_BUFFER_VIEW IbView = { m_IBBuffer->GetGpuVirtualAddress() , m_IBBuffer->GetBufferSize() , DXGI_FORMAT_R16_UINT };
		m_Game->GetCommandList()->IASetIndexBuffer(&IbView);
		m_Game->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_Game->GetCommandList()->DrawIndexedInstanced(m_Triangles.size()*3, 1, 0, 0, 0);
	}
}

HWD3DMesh_DX12::HWD3DMesh_DX12(class HWD3DGame_DX12* InGame, const char* InFilename)
	: m_Game(InGame)
{
	LoadMeshFile(InFilename);
	CreateBuffers();
}

HWD3DMesh_DX12::~HWD3DMesh_DX12()
{
	HWD3D_SafeRelease(m_IBBuffer);
	HWD3D_SafeRelease(m_VBBuffer);
}

bool HWD3DMesh_DX12::CreateBuffers()
{
	ID3D12Device* Dev = m_Game ? m_Game->GetDevice() : nullptr;

	if (!Dev)
	{
		return false;
	}

	// Vertex Buffer:
	{
		const int BufferSize = m_Vertices.size()*sizeof(hwd3d_vertex);
		m_VBBuffer = HWD3DBufferConstant_DX12::CreateConstantBuffer(hwd3d_constant_buffer_t::VertexBuffer, BufferSize, *Dev);
		if (m_VBBuffer)
		{
			m_VBBuffer->SetBufferData(m_Vertices.data(), BufferSize);
		}
	}
	

	// Fill index Buffer
	{
		std::vector<hwd3d_graphics_index> Indices;
		Indices.reserve(m_Triangles.size()*3);

		for (int i = 0; i < static_cast<int>(m_Triangles.size()); i++)
		{
			Indices.push_back(m_Triangles[i].v1);
			Indices.push_back(m_Triangles[i].v2);
			Indices.push_back(m_Triangles[i].v3);
		}

		const int BufferSize = Indices.size()*sizeof(hwd3d_graphics_index);
		m_IBBuffer = HWD3DBufferConstant_DX12::CreateConstantBuffer(hwd3d_constant_buffer_t::IndexBuffer, BufferSize, *Dev);
		if (m_IBBuffer)
		{
			m_IBBuffer->SetBufferData(Indices.data(), BufferSize);
		}
	}

	return true;
}
