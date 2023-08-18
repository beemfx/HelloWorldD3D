// D3D Hello World

#include "HWD3DMesh_DX6.h"
#include "HWD3DGame_DX6.h"

HWD3DMesh* HWD3DMesh::CreateMesh(class HWD3DGame* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX6(dynamic_cast<HWD3DGame_DX6*>(InGame), InFilename);
}

void HWD3DMesh_DX6::Draw()
{
	if (m_Game && m_Game->GetDevice() && m_VB)
	{ 
		m_Game->GetDevice()->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, m_VB, m_D3DIndexes.data(), m_D3DIndexes.size(), D3DDP_WAIT);
	}
}

HWD3DMesh_DX6::HWD3DMesh_DX6(class HWD3DGame_DX6* InGame, const char* InFilename)
	: m_Game(InGame)
{
	LoadMeshFile(InFilename);
	CreateBuffers();
}

HWD3DMesh_DX6::~HWD3DMesh_DX6()
{
	HWD3D_SafeRelease(m_VB);
}

bool HWD3DMesh_DX6::CreateBuffers()
{
	m_D3DIndexes.resize(0);

	IDirect3D3* D3D = m_Game ? m_Game->GetD3D() : nullptr;

	if (!D3D)
	{
		return false;
	}

	D3DVERTEXBUFFERDESC Vbd = { };
	Vbd.dwSize = sizeof(Vbd);
	Vbd.dwCaps = 0;
	Vbd.dwFVF = m_FVF;
	Vbd.dwNumVertices = m_Vertices.size();
	const HRESULT CvbRes = D3D->CreateVertexBuffer(&Vbd, &m_VB, 0, NULL);
	if (FAILED(CvbRes) || !m_VB)
	{
		return false;
	}

	D3DVERTEX* DstVerts = nullptr;
	DWORD Size = 0;
	const HRESULT LockRes = m_VB->Lock(0, reinterpret_cast<LPVOID*>(&DstVerts), &Size);
	if (SUCCEEDED(LockRes) && DstVerts)
	{
		for (int i = 0; i < static_cast<int>(m_Vertices.size()); i++)
		{
			D3DVERTEX NewVert = *reinterpret_cast<const D3DVERTEX*>(&m_Vertices[i]);
			D3DVERTEX& Dest = DstVerts[i];
			Dest = NewVert;
		}

		m_VB->Unlock();
	}

	for (const auto& T : m_Triangles)
	{
		m_D3DIndexes.push_back(T.v1);
		m_D3DIndexes.push_back(T.v2);
		m_D3DIndexes.push_back(T.v3);
	}

	return true;
}
