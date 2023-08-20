// D3D Hello World

#include "HWD3DMesh_DX10.h"
#include "HWD3DGame_DX10.h"

HWD3DMesh* HWD3DMesh::CreateMesh(class HWD3DGame* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX10(dynamic_cast<HWD3DGame_DX10*>(InGame), InFilename);
}

void HWD3DMesh_DX10::Draw()
{
	if (m_Game && m_Game->GetDevice() && m_VB && m_IB)
	{ 
		static_assert(sizeof(hwd3d_vertex) == (sizeof(float)*8), "hwd3d_vertex has padding.");
		ID3D10Buffer* Buffers[] = { m_VB };
		const UINT Strides[] = { sizeof(hwd3d_vertex) };
		const UINT Offsets[] = { 0 };
		m_Game->GetDevice()->IASetVertexBuffers(0, _countof(Buffers), Buffers, Strides, Offsets);
		m_Game->GetDevice()->IASetIndexBuffer(m_IB, DXGI_FORMAT_R16_UINT, 0);
		m_Game->GetDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_Game->GetDevice()->DrawIndexed(m_Triangles.size()*3, 0, 0);
	}
}

HWD3DMesh_DX10::HWD3DMesh_DX10(class HWD3DGame_DX10* InGame, const char* InFilename)
	: m_Game(InGame)
{
	LoadMeshFile(InFilename);
	CreateBuffers();
}

HWD3DMesh_DX10::~HWD3DMesh_DX10()
{
	HWD3D_SafeRelease(m_IB);
	HWD3D_SafeRelease(m_VB);
}

bool HWD3DMesh_DX10::CreateBuffers()
{
	ID3D10Device* Dev = m_Game ? m_Game->GetDevice() : nullptr;

	if (!Dev)
	{
		return false;
	}

	const D3D10_USAGE Usage = D3D10_USAGE_IMMUTABLE;
	
	// Fill Vertex Buffer
	{
		const UINT VbSize = m_Vertices.size()*sizeof(hwd3d_vertex);
		D3D10_BUFFER_DESC Bd = { };
		Bd.Usage = Usage;
		Bd.ByteWidth = VbSize;
		Bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;

		D3D10_SUBRESOURCE_DATA Data = { };
		Data.pSysMem = m_Vertices.data();
		const HRESULT CvbRes = Dev->CreateBuffer(&Bd, &Data, &m_VB);
		if (FAILED(CvbRes) || !m_VB)
		{
			return false;
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

		const UINT IbSize = Indices.size()*sizeof(hwd3d_graphics_index);
		D3D10_BUFFER_DESC Bd = { };
		Bd.Usage = Usage;
		Bd.ByteWidth = IbSize;
		Bd.BindFlags = D3D10_BIND_INDEX_BUFFER;

		D3D10_SUBRESOURCE_DATA Data = { };
		Data.pSysMem = Indices.data();
		const HRESULT CvbRes = Dev->CreateBuffer(&Bd, &Data, &m_IB);
		if (FAILED(CvbRes) || !m_IB)
		{
			return false;
		}
	}

	return true;
}
