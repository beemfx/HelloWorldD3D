// D3D Hello World

#pragma once

// This is all taken form d3dmacs.h but made to allocate it's own exec buffer so one does not
// have to worry about preallocating. Note that various macros were renamed to look pretty
// but some still have the original names, which look ugly, but I'm keeping them for completeness.
//
// You could certainly go more advanced here and not require calling BeginInstructions, BeginTransformStates,
// or BeginRenderStates, but for a Hello World program that's maybe overkill.

#include "HWD3DObject.h"
#include <d3d.h>

class HWD3DExecBuffer_DX2 : public HWD3DObject
{
private:
	
	std::vector<BYTE> m_ExecBufferData;
	class HWD3DGame_DX2* const m_Game;
	IDirect3DExecuteBuffer* m_ExecBuffer = nullptr;
	D3DEXECUTEBUFFERDESC m_ExecBufferDesc = {};
	DWORD m_NumVerts = 0;
	DWORD m_DataStart = 0;
	DWORD m_InsStart = 0;
	DWORD m_NumRenderStatesAdded = 0;
	DWORD m_RenderStateCount = 0;
	DWORD m_NumTransformStatesAdded = 0;
	DWORD m_TransformStateCount = 0;

public:

	HWD3DExecBuffer_DX2(class HWD3DGame_DX2* InGame);
	virtual void OnObjectDestruct() override;

	void BeginInstructions();
	void FinalizeBuffer();

	void ExecuteBuffer();

private:
	
	void PutD3DInstruction(BYTE op, BYTE sz, WORD cnt)
	{
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DINSTRUCTION);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DINSTRUCTION)ptr)->bOpcode = op;
		((LPD3DINSTRUCTION)ptr)->bSize = sz;
		((LPD3DINSTRUCTION)ptr)->wCount = cnt;
	}

public:
	
	void SetVertexData(const void* loc, DWORD cnt)
	{
		assert(m_ExecBufferData.size() == 0); // Vertex data must be the first thing set.
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DVERTEX) * cnt;
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		memcpy(ptr, loc, OpSize);

		m_NumVerts += cnt;
	}

private:
	
	void OP_MATRIX_MULTIPLY(WORD cnt)
	{
		PutD3DInstruction(D3DOP_MATRIXMULTIPLY, sizeof(D3DMATRIXMULTIPLY), cnt);
	}

	void MATRIX_MULTIPLY_DATA(D3DMATRIXHANDLE src1, D3DMATRIXHANDLE src2, D3DMATRIXHANDLE dest)
	{
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DMATRIXMULTIPLY);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DMATRIXMULTIPLY)ptr)->hSrcMatrix1 = src1;
		((LPD3DMATRIXMULTIPLY)ptr)->hSrcMatrix2 = src2;
		((LPD3DMATRIXMULTIPLY)ptr)->hDestMatrix = dest;
	}

	void OP_STATE_LIGHT(WORD cnt)
	{
		PutD3DInstruction(D3DOP_STATELIGHT, sizeof(D3DSTATE), cnt);
	}

public:
	
	void BeginTransformStates(WORD cnt)
	{
		assert(m_NumTransformStatesAdded == 0 && m_TransformStateCount == 0); // Only set transform states once.
		m_TransformStateCount = cnt;
		PutD3DInstruction(D3DOP_STATETRANSFORM, sizeof(D3DSTATE), cnt);
	}

	void SetTransformState(D3DTRANSFORMSTATETYPE type, DWORD arg)
	{
		m_NumTransformStatesAdded++;
		assert(m_NumTransformStatesAdded <= m_TransformStateCount);

		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DSTATE);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DSTATE)ptr)->dtstTransformStateType = type;
		((LPD3DSTATE)ptr)->dwArg[0] = arg;
	}

	void BeginRenderStates(WORD cnt)
	{
		assert(m_NumRenderStatesAdded == 0 && m_RenderStateCount == 0); // Only set render states once.

		m_RenderStateCount = cnt;
		PutD3DInstruction(D3DOP_STATERENDER, sizeof(D3DSTATE), cnt);
	}

	void SetRenderState(D3DRENDERSTATETYPE type, DWORD arg)
	{
		m_NumRenderStatesAdded++;
		assert(m_NumRenderStatesAdded <= m_RenderStateCount);

		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DSTATE);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DSTATE)ptr)->drstRenderStateType = type;
		((LPD3DSTATE)ptr)->dwArg[0] = arg;
	}

private:
	
	void OP_PROCESS_VERTICES(WORD cnt)
	{
		PutD3DInstruction(D3DOP_PROCESSVERTICES, sizeof(D3DPROCESSVERTICES), cnt);
	}

public:
	
	void ProcessVertexes(DWORD flgs, WORD strt, DWORD cnt)
	{
		OP_PROCESS_VERTICES(1);

		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DPROCESSVERTICES);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DPROCESSVERTICES)ptr)->dwFlags = flgs;
		((LPD3DPROCESSVERTICES)ptr)->wStart = strt;
		((LPD3DPROCESSVERTICES)ptr)->wDest = strt;
		((LPD3DPROCESSVERTICES)ptr)->dwCount = cnt;
		((LPD3DPROCESSVERTICES)ptr)->dwReserved = 0;
	}

	void BeginTriangleList(WORD cnt)
	{
		// Make sure that the triangle data (not OP) will be QWORD aligned
		if (QWORD_ALIGNED())
		{
			OP_NOP();
		}

		PutD3DInstruction(D3DOP_TRIANGLE, sizeof(D3DTRIANGLE), cnt);
	}

	void AddTriangle(WORD v1, WORD v2, WORD v3, WORD Flags)
	{
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DTRIANGLE);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DTRIANGLE)ptr)->v1 = v1;
		((LPD3DTRIANGLE)ptr)->v2 = v2;
		((LPD3DTRIANGLE)ptr)->v3 = v3;
		((LPD3DTRIANGLE)ptr)->wFlags = Flags;
	}

private:

	void TRIANGLE_LIST_DATA(const void* loc, DWORD count)
	{
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DTRIANGLE) * count;
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		memcpy(ptr, loc, OpSize);
	}

	void OP_LINE_LIST(WORD cnt)
	{
		PutD3DInstruction(D3DOP_LINE, sizeof(D3DLINE), cnt);
	}

	void LINE_LIST_DATA(const void* loc, DWORD count)
	{
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DLINE) * count;
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		memcpy(ptr, loc, OpSize);
	}

	void OP_POINT_LIST(WORD first, WORD cnt)
	{
		PutD3DInstruction(D3DOP_POINT, sizeof(D3DPOINT), 1);

		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DPOINT);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DPOINT)(ptr))->wCount = cnt;
		((LPD3DPOINT)(ptr))->wFirst = first;
	}

	void OP_SPAN_LIST(WORD first, WORD cnt)
	{
		PutD3DInstruction(D3DOP_SPAN, sizeof(D3DSPAN), 1);

		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DSPAN);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DSPAN)(ptr))->wCount = cnt;
		((LPD3DSPAN)(ptr))->wFirst = first;
	}

	void OP_BRANCH_FORWARD(DWORD tmask, DWORD tvalue, BOOL tnegate, DWORD toffset)
	{
		PutD3DInstruction(D3DOP_BRANCHFORWARD, sizeof(D3DBRANCH), 1);

		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DBRANCH);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DBRANCH)ptr)->dwMask = tmask;
		((LPD3DBRANCH)ptr)->dwValue = tvalue;
		((LPD3DBRANCH)ptr)->bNegate = tnegate;
		((LPD3DBRANCH)ptr)->dwOffset = toffset;
	}

	void SetStatus(DWORD flags, DWORD status, LONG _x1, LONG _y1, LONG _x2, LONG _y2)
	{
		PutD3DInstruction(D3DOP_SETSTATUS, sizeof(D3DSTATUS), 1);

		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DSTATUS);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DSTATUS)(ptr))->dwFlags = flags;
		((LPD3DSTATUS)(ptr))->dwStatus = status;
		((LPD3DSTATUS)(ptr))->drExtent.x1 = _x1;
		((LPD3DSTATUS)(ptr))->drExtent.y1 = _y1;
		((LPD3DSTATUS)(ptr))->drExtent.x2 = _x2;
		((LPD3DSTATUS)(ptr))->drExtent.y2 = _y2;
	}

private:
	
	void OP_NOP()
	{
		PutD3DInstruction(D3DOP_TRIANGLE, sizeof(D3DTRIANGLE), 0);
	}
	
	void OP_EXIT()
	{
		PutD3DInstruction(D3DOP_EXIT, 0, 0);
	}

	bool QWORD_ALIGNED()
	{
		const size_t Start = m_ExecBufferData.size();
		return !(0x00000007L & (ULONG)(Start));
	}
};
