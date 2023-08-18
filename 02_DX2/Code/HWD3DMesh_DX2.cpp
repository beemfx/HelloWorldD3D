// D3D Hello World

#include "HWD3DMesh_DX2.h"
#include "HWD3DGame_DX2.h"
#include "d3dmacs.h"

HWD3DMesh* HWD3DMesh::CreateMesh(class HWD3DGame* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX2(dynamic_cast<HWD3DGame_DX2*>(InGame), InFilename);
}

void HWD3DMesh_DX2::Draw()
{
	if (m_ExecBuffer && m_Game && m_Game->GetDevice() && m_Game->GetViewport())
	{
		const HRESULT ExecRes = m_Game->GetDevice()->Execute(m_ExecBuffer, m_Game->GetViewport(), D3DEXECUTE_CLIPPED);
		assert(SUCCEEDED(ExecRes));
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

	m_ExecBufferDesc.dwSize = sizeof(m_ExecBufferDesc);
	m_ExecBufferDesc.dwFlags = D3DDEB_BUFSIZE;
	static const int NUM_INSTR = 4;
	static const int NUM_RENDER_STATES = 0;
	m_ExecBufferDesc.dwBufferSize 
		= sizeof(D3DVERTEX)*m_Vertices.size()
		+ sizeof(D3DINSTRUCTION)*NUM_INSTR 
		+ sizeof(D3DSTATE)*NUM_RENDER_STATES 
		+ sizeof(D3DPROCESSVERTICES)
		+ sizeof(D3DTRIANGLE)*m_Triangles.size();

	const HRESULT CreateExecBufferRes = m_Game->GetDevice()->CreateExecuteBuffer(&m_ExecBufferDesc, &m_ExecBuffer, nullptr);
	if (FAILED(CreateExecBufferRes) || !m_ExecBuffer)
	{
		return false;
	}

	if (SUCCEEDED(m_ExecBuffer->Lock(&m_ExecBufferDesc)))
	{
		memset(m_ExecBufferDesc.lpData, 0, m_ExecBufferDesc.dwBufferSize);

		LPVOID lpBufStart = m_ExecBufferDesc.lpData;
		LPVOID lpPointer = lpBufStart;

		VERTEX_DATA(m_Vertices.data(), m_Vertices.size(), lpPointer); // Out data is directly compatible with D3DVERTEX.

		LPVOID lpInsStart = lpPointer;

		// If we actually wanted to obtain the status we might want this:
		// OP_SET_STATUS(D3DSETSTATUS_ALL, D3DSTATUS_DEFAULT, 2048, 2048, 0, 0, lpPointer);

		OP_PROCESS_VERTICES(1, lpPointer);
		PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, 0, m_Vertices.size(), lpPointer);

		// Make sure that the triangle data (not OP) will be QWORD aligned
		if (QWORD_ALIGNED(lpPointer))
		{
			OP_NOP(lpPointer);
		}

		OP_TRIANGLE_LIST(static_cast<WORD>(m_Triangles.size()), lpPointer);
		for (int i = 0; i < static_cast<int>(m_Triangles.size()); i++)
		{
			((LPD3DTRIANGLE)lpPointer)->v1 = m_Triangles[i].v1;
			((LPD3DTRIANGLE)lpPointer)->v2 = m_Triangles[i].v2;
			((LPD3DTRIANGLE)lpPointer)->v3 = m_Triangles[i].v3;
			((LPD3DTRIANGLE)lpPointer)->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE;

			lpPointer = (void*)(((LPD3DTRIANGLE)lpPointer) + 1);
		}

		OP_EXIT(lpPointer);

		const ULONG SizeWritten = (ULONG)((char*)lpPointer - (char*)lpBufStart);
		// Do a sanity check to make sure our math was correct buffer is the right size ( the min subtracts sizeof(D3DINSTRUCTION) because we might not have written the NOP)
		assert(((m_ExecBufferDesc.dwBufferSize - sizeof(D3DINSTRUCTION)) <= SizeWritten) && (SizeWritten <= m_ExecBufferDesc.dwBufferSize));

		const HRESULT UnlockRes = m_ExecBuffer->Unlock();
		if (FAILED(UnlockRes))
		{
			return false;
		}

		D3DEXECUTEDATA ExecData = {};
		ExecData.dwSize = sizeof(ExecData);
		ExecData.dwInstructionOffset = (ULONG)((char*)lpInsStart - (char*)lpBufStart);
		ExecData.dwInstructionLength = (ULONG)((char*)lpPointer - (char*)lpInsStart);
		ExecData.dwVertexCount = m_Vertices.size();
		const HRESULT SetDataRes = m_ExecBuffer->SetExecuteData(&ExecData);
		if (FAILED(SetDataRes))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}
