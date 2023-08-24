// D3D Hello World

#include "HWD3DMesh_DX12.h"
#include "HWD3DGame_DX12.h"

HWD3DMesh* HWD3DMesh::CreateMesh(class HWD3DGame* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX12(dynamic_cast<HWD3DGame_DX12*>(InGame), InFilename);
}

void HWD3DMesh_DX12::Draw()
{
	if (m_Game && m_Game->GetCommandList() && m_VBBuffer.IsValid() && m_IBBuffer.IsValid())
	{
		m_VBBuffer.PrepareForDraw(*m_Game->GetCommandList(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		m_IBBuffer.PrepareForDraw(*m_Game->GetCommandList(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

		const D3D12_VERTEX_BUFFER_VIEW VbViews[] = { { m_VBBuffer.GetGpuVirtualAddress() , m_VBBuffer.GetBufferByteSize() , sizeof(hwd3d_vertex) }, };
		m_Game->GetCommandList()->IASetVertexBuffers(0, _countof(VbViews), VbViews);
		const D3D12_INDEX_BUFFER_VIEW IbView = { m_IBBuffer.GetGpuVirtualAddress() , m_IBBuffer.GetBufferByteSize() , DXGI_FORMAT_R16_UINT };
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
	m_IBBuffer.Deinit();
	m_VBBuffer.Deinit();
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
		m_VBBuffer.Init(Dev, BufferSize);
		m_VBBuffer.SetBufferData(m_Vertices.data(), BufferSize);
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
		
		m_IBBuffer.Init(Dev, BufferSize);
		m_IBBuffer.SetBufferData(Indices.data(), BufferSize);
	}

	return true;
}
