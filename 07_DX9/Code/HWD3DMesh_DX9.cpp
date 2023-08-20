// D3D Hello World

#include "HWD3DMesh_DX9.h"
#include "HWD3DGame_DX9.h"

HWD3DMesh* HWD3DMesh::CreateMesh(class HWD3DGame* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX9(dynamic_cast<HWD3DGame_DX9*>(InGame), InFilename);
}

void HWD3DMesh_DX9::Draw()
{
	if (m_Game && m_Game->GetDevice() && m_VB && m_IB)
	{ 
		static_assert(sizeof(hwd3d_vertex) == (sizeof(float)*8), "hwd3d_vertex has padding.");
		m_Game->GetDevice()->SetStreamSource(0, m_VB, sizeof(hwd3d_vertex));
		m_Game->GetDevice()->SetIndices(m_IB, 0);
		m_Game->GetDevice()->SetVertexShader(m_FVF);
		m_Game->GetDevice()->SetPixelShader(0);
		const HRESULT Res = m_Game->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_Vertices.size(), 0, m_Triangles.size());
		assert(SUCCEEDED(Res));
	}
}

HWD3DMesh_DX9::HWD3DMesh_DX9(class HWD3DGame_DX9* InGame, const char* InFilename)
	: m_Game(InGame)
{
	LoadMeshFile(InFilename);
	CreateBuffers();
}

HWD3DMesh_DX9::~HWD3DMesh_DX9()
{
	HWD3D_SafeRelease(m_IB);
	HWD3D_SafeRelease(m_VB);
}

bool HWD3DMesh_DX9::CreateBuffers()
{
	IDirect3DDevice8* Dev = m_Game ? m_Game->GetDevice() : nullptr;

	if (!Dev)
	{
		return false;
	}

	const DWORD Usage = 0;// D3DUSAGE_DYNAMIC;
	
	const UINT VbSize = m_Vertices.size()*sizeof(hwd3d_vertex);
	const HRESULT CvbRes = Dev->CreateVertexBuffer(VbSize, Usage, m_FVF, D3DPOOL_DEFAULT, &m_VB);
	if (FAILED(CvbRes) || !m_VB)
	{
		return false;
	}

	// Fill Vertex Buffer
	{
		hwd3d_vertex* DstVerts = nullptr;
		const HRESULT LockRes = m_VB->Lock(0, 0, reinterpret_cast<BYTE**>(&DstVerts), 0);
		if (SUCCEEDED(LockRes) && DstVerts)
		{
			memcpy(DstVerts, m_Vertices.data(), VbSize);
			m_VB->Unlock();
		}
	}

	const HRESULT CibRes = Dev->CreateIndexBuffer(m_Triangles.size()*3*sizeof(hwd3d_graphics_index), Usage, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_IB);
	if (FAILED(CibRes) || !m_IB)
	{
		return false;
	}

	// Fill Index Buffer
	{
		static_assert(sizeof(hwd3d_graphics_index) == 2, "Padding on hwd3d_graphics_index");
		hwd3d_graphics_index* DstIdxs = nullptr;
		const HRESULT LockRes = m_IB->Lock(0, m_Triangles.size()*3*sizeof(hwd3d_graphics_index), reinterpret_cast<BYTE**>(&DstIdxs), 0);
		if (SUCCEEDED(LockRes) && DstIdxs)
		{
			for (int i = 0; i < static_cast<int>(m_Triangles.size()); i++)
			{
				DstIdxs[i*3+0] = m_Triangles[i].v1;
				DstIdxs[i*3+1] = m_Triangles[i].v2;
				DstIdxs[i*3+2] = m_Triangles[i].v3;
			}

			m_VB->Unlock();
		}
	}

	return true;
}
