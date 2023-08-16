// D3D Hello World

#include "HWD3DMesh_DX2.h"
#include "HWD3DGame_DX2.h"
#include "d3dmacs.h"

/* Cube vertices, normals, shades, and modeling transform */
static int NumVertices = 24;
static D3DVERTEX CubeVertices[] =
{
	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0) },
	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0) },

	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0) },
	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0) },

	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0) },
	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0) },

	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0) },
	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0) },

	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0) },
	{ D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0) },

	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0) },
	{ D3DVAL(1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0) },
	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0) },
	{ D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0) }
};

//*** Cube edges - ordered indices into the vertex array
static const int NumTri = 12;
static const int CubeTri[] =
{
	0, 1, 2, 0, 2, 3,
	4, 5, 6, 4, 6, 7,
	8, 9, 10, 8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23
};

HWD3DMesh_DX2* HWD3DMesh_DX2::CreateMesh(class HWD3DGame_DX2* InGame, const char* InFilename)
{
	return new HWD3DMesh_DX2(InGame, InFilename);
}

void HWD3DMesh_DX2::Release()
{
	delete this;
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
	CreateExecBuffer();
}

HWD3DMesh_DX2::~HWD3DMesh_DX2()
{
	if (m_ExecBuffer)
	{
		m_ExecBuffer->Release();
		m_ExecBuffer = nullptr;
	}
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
		= sizeof(D3DVERTEX)*NumVertices
		+ sizeof(D3DINSTRUCTION)*NUM_INSTR 
		+ sizeof(D3DSTATE)*NUM_RENDER_STATES 
		+ sizeof(D3DPROCESSVERTICES)
		+ sizeof(D3DTRIANGLE)*NumTri;

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

		VERTEX_DATA(&CubeVertices[0], NumVertices, lpPointer);

		LPVOID lpInsStart = lpPointer;

		// If we actually wanted to obtain the status we might want this:
		// OP_SET_STATUS(D3DSETSTATUS_ALL, D3DSTATUS_DEFAULT, 2048, 2048, 0, 0, lpPointer);

		OP_PROCESS_VERTICES(1, lpPointer);
		PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, 0, NumVertices, lpPointer);

		// Make sure that the triangle data (not OP) will be QWORD aligned
		if (QWORD_ALIGNED(lpPointer))
		{
			OP_NOP(lpPointer);
		}

		OP_TRIANGLE_LIST(NumTri, lpPointer);
		for (int i = 0; i < NumTri; i++)
		{
			((LPD3DTRIANGLE)lpPointer)->v1 = CubeTri[i * 3];
			((LPD3DTRIANGLE)lpPointer)->v2 = CubeTri[i * 3 + 1];
			((LPD3DTRIANGLE)lpPointer)->v3 = CubeTri[i * 3 + 2];
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
		ExecData.dwVertexCount = NumVertices;
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
