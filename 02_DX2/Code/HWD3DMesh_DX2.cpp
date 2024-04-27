// D3D Hello World

#include "HWD3DMesh_DX2.h"
#include "HWD3DGame_DX2.h"

HWD3DMesh* HWD3DMesh::CreateMesh(class HWD3DGame* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX2(dynamic_cast<HWD3DGame_DX2*>(InGame), InFilename);
}

void HWD3DMesh_DX2::Draw()
{
	if (m_ExecBuffer)
	{
		m_ExecBuffer->ExecuteBuffer();
	}
}

HWD3DMesh_DX2::HWD3DMesh_DX2(class HWD3DGame_DX2* InGame, const char* InFilename)
	: m_Game(InGame)
{
	LoadMeshFile(InFilename);
	CreateExecBuffer();
}

HWD3DMesh_DX2::~HWD3DMesh_DX2()
{
	HWD3D_SafeRelease(m_ExecBuffer);
}

bool HWD3DMesh_DX2::CreateExecBuffer()
{
	if (!m_Game || !m_Game->GetDevice())
	{
		return false;
	}

	m_ExecBuffer = new HWD3DExecBuffer_DX2(m_Game);

	if (m_ExecBuffer)
	{
		m_ExecBuffer->BeginData();
		m_ExecBuffer->VERTEX_DATA(m_Vertices.data(), m_Vertices.size()); // Out data is directly compatible with D3DVERTEX.

		m_ExecBuffer->BeginInstructions();
		m_ExecBuffer->OP_PROCESS_VERTICES(1);
		// If we actually wanted to obtain the status we might want this:
		// m_ExecBuffer->OP_SET_STATUS(D3DSETSTATUS_ALL, D3DSTATUS_DEFAULT, 2048, 2048, 0, 0, lpPointer);
		m_ExecBuffer->PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, 0, static_cast<DWORD>(m_Vertices.size()));

		m_ExecBuffer->OP_TRIANGLE_LIST(static_cast<WORD>(m_Triangles.size()));
		for (int i = 0; i < static_cast<int>(m_Triangles.size()); i++)
		{
			m_ExecBuffer->ADD_TRIANGLE(m_Triangles[i].v1, m_Triangles[i].v2, m_Triangles[i].v3, D3DTRIFLAG_EDGEENABLETRIANGLE);
		}

		m_ExecBuffer->FinalizeBuffer();
	}
	else
	{
		return false;
	}

	return true;
}
