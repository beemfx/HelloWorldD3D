// D3D Hello World

#include "HWD3DMesh_DX5.h"
#include "HWD3DGame_DX5.h"

HWD3DMesh* HWD3DMesh::CreateMesh(class HWD3DGame* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX5(dynamic_cast<HWD3DGame_DX5*>(InGame), InFilename);
}

void HWD3DMesh_DX5::Draw()
{
	if (m_Game && m_Game->GetDevice())
	{
		/*
		const DWORD VertAddr = reinterpret_cast<unsigned long>(m_D3DVertices.data());
		assert( (VertAddr%2) == 0 );
		const DWORD IdxAddr = reinterpret_cast<DWORD>(m_D3DIndexes.data());
		assert( (IdxAddr%2) == 0 );
		*/
		m_Game->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DVT_VERTEX, m_D3DVertices.data(), m_D3DVertices.size(), m_D3DIndexes.data(), m_D3DIndexes.size(), D3DDP_WAIT); 
	}
}

HWD3DMesh_DX5::HWD3DMesh_DX5(class HWD3DGame_DX5* InGame, const char* InFilename)
	: m_Game(InGame)
{
	LoadMeshFile(InFilename);
	CreateBuffers();
}

HWD3DMesh_DX5::~HWD3DMesh_DX5()
{
	
}

bool HWD3DMesh_DX5::CreateBuffers()
{
	m_D3DIndexes.resize(0);
	m_D3DVertices.resize(0);

	for (const auto& V : m_Vertices)
	{
		D3DVERTEX NewVert = *reinterpret_cast<const D3DVERTEX*>(&V);
		m_D3DVertices.push_back(NewVert);
	}

	for (const auto& T : m_Triangles)
	{
		m_D3DIndexes.push_back(T.v1);
		m_D3DIndexes.push_back(T.v2);
		m_D3DIndexes.push_back(T.v3);
	}

	return true;
}
