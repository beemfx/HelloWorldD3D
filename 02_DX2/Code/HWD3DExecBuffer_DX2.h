// D3D Hello World

#pragma once

// This is all taken form d3dmacs.h but made to allocate it's own exec buffer so one does not
// have to worry about preallocating.

#include "HWD3DObject.h"
#include <d3d.h>

class HWD3DExecBuffer_DX2 : public HWD3DObject
{
private:

	class Allocator : public std::allocator<BYTE>
	{
	public:

		value_type* allocate(std::size_t n)
		{
			return static_cast<value_type*>(::operator new (n * sizeof(value_type)));
		}

		void deallocate(value_type* p, std::size_t) noexcept  // Use pointer if pointer is not a value_type*
		{
			::operator delete(p);
		}
	};

	std::vector<BYTE, Allocator> m_ExecBufferData;
	class HWD3DGame_DX2* const m_Game;
	IDirect3DExecuteBuffer* m_ExecBuffer = nullptr;
	D3DEXECUTEBUFFERDESC m_ExecBufferDesc = {};
	DWORD m_NumVerts = 0;
	DWORD m_DataStart = 0;
	DWORD m_InsStart = 0;

public:

	HWD3DExecBuffer_DX2(class HWD3DGame_DX2* InGame);
	virtual void OnObjectDestruct() override;

	void BeginData();
	void BeginInstructions();
	void FinalizeBuffer();

	void ExecuteBuffer();

	void PUTD3DINSTRUCTION(BYTE op, BYTE sz, WORD cnt)
	{
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DINSTRUCTION);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DINSTRUCTION)ptr)->bOpcode = op;
		((LPD3DINSTRUCTION)ptr)->bSize = sz;
		((LPD3DINSTRUCTION)ptr)->wCount = cnt;
	}

	void VERTEX_DATA(const void* loc, DWORD cnt)
	{
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DVERTEX) * cnt;
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		memcpy(ptr, loc, OpSize);

		m_NumVerts += cnt;
	}

	// OP_MATRIX_MULTIPLY size: 4 (sizeof D3DINSTRUCTION)
	void OP_MATRIX_MULTIPLY(WORD cnt)
	{
		PUTD3DINSTRUCTION(D3DOP_MATRIXMULTIPLY, sizeof(D3DMATRIXMULTIPLY), cnt);
	}

	// MATRIX_MULTIPLY_DATA size: 12 (sizeof MATRIXMULTIPLY)
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

	// OP_STATE_LIGHT size: 4 (sizeof D3DINSTRUCTION)
	void OP_STATE_LIGHT(WORD cnt)
	{
		PUTD3DINSTRUCTION(D3DOP_STATELIGHT, sizeof(D3DSTATE), cnt);
	}

	// OP_STATE_TRANSFORM size: 4 (sizeof D3DINSTRUCTION)
	void OP_STATE_TRANSFORM(WORD cnt)
	{
		PUTD3DINSTRUCTION(D3DOP_STATETRANSFORM, sizeof(D3DSTATE), cnt);
	}

	// OP_STATE_RENDER size: 4 (sizeof D3DINSTRUCTION)
	void OP_STATE_RENDER(WORD cnt)
	{
		PUTD3DINSTRUCTION(D3DOP_STATERENDER, sizeof(D3DSTATE), cnt);
	}

	// STATE_DATA size: 8 (sizeof D3DSTATE)
	void STATE_DATA(DWORD type, DWORD arg)
	{
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DSTATE);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DSTATE)ptr)->drstRenderStateType = static_cast<D3DRENDERSTATETYPE>(type);
		((LPD3DSTATE)ptr)->dwArg[0] = arg;
	}

	// OP_PROCESS_VERTICES size: 4 (sizeof D3DINSTRUCTION)
	void OP_PROCESS_VERTICES(WORD cnt)
	{
		PUTD3DINSTRUCTION(D3DOP_PROCESSVERTICES, sizeof(D3DPROCESSVERTICES), cnt);
	}

	// PROCESSVERTICES_DATA size: 16 (sizeof D3DPROCESSVERTICES)
	void PROCESSVERTICES_DATA(DWORD flgs, WORD strt, DWORD cnt)
	{
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

	// OP_TRIANGLE_LIST size: 4 (sizeof D3DINSTRUCTION)
	void OP_TRIANGLE_LIST(WORD cnt)
	{
		PUTD3DINSTRUCTION(D3DOP_TRIANGLE, sizeof(D3DTRIANGLE), cnt);

		// Make sure that the triangle data (not OP) will be QWORD aligned
		if (!QWORD_ALIGNED())
		{
			OP_NOP();
		}
	}

	void ADD_TRIANGLE(WORD v1, WORD v2, WORD v3, WORD Flags)
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

	void TRIANGLE_LIST_DATA(const void* loc, DWORD count)
	{
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DTRIANGLE) * count;
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		memcpy(ptr, loc, OpSize);
	}

	// OP_LINE_LIST size: 4 (sizeof D3DINSTRUCTION)
	void OP_LINE_LIST(WORD cnt)
	{
		PUTD3DINSTRUCTION(D3DOP_LINE, sizeof(D3DLINE), cnt);
	}

	void LINE_LIST_DATA(const void* loc, DWORD count)
	{
		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DLINE) * count;
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		memcpy(ptr, loc, OpSize);
	}

	// OP_POINT_LIST size: 8 (sizeof D3DINSTRUCTION + sizeof D3DPOINT)
	void OP_POINT_LIST(WORD first, WORD cnt)
	{
		PUTD3DINSTRUCTION(D3DOP_POINT, sizeof(D3DPOINT), 1);

		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DPOINT);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DPOINT)(ptr))->wCount = cnt;
		((LPD3DPOINT)(ptr))->wFirst = first;
	}

	// OP_SPAN_LIST size: 8 (sizeof D3DINSTRUCTION + sizeof D3DSPAN)
	void OP_SPAN_LIST(WORD first, WORD cnt)
	{
		PUTD3DINSTRUCTION(D3DOP_SPAN, sizeof(D3DSPAN), 1);

		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DSPAN);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DSPAN)(ptr))->wCount = cnt;
		((LPD3DSPAN)(ptr))->wFirst = first;
	}

	// OP_BRANCH_FORWARD size: 18 (sizeof D3DINSTRUCTION + sizeof D3DBRANCH)
	void OP_BRANCH_FORWARD(DWORD tmask, DWORD tvalue, BOOL tnegate, DWORD toffset)
	{
		PUTD3DINSTRUCTION(D3DOP_BRANCHFORWARD, sizeof(D3DBRANCH), 1);

		const size_t Start = m_ExecBufferData.size();
		const size_t OpSize = sizeof(D3DBRANCH);
		m_ExecBufferData.resize(Start + OpSize);
		void* ptr = &m_ExecBufferData[Start];

		((LPD3DBRANCH)ptr)->dwMask = tmask;
		((LPD3DBRANCH)ptr)->dwValue = tvalue;
		((LPD3DBRANCH)ptr)->bNegate = tnegate;
		((LPD3DBRANCH)ptr)->dwOffset = toffset;
	}

	// OP_SET_STATUS size: 20 (sizeof D3DINSTRUCTION + sizeof D3DSTATUS)
	void OP_SET_STATUS(DWORD flags, DWORD status, LONG _x1, LONG _y1, LONG _x2, LONG _y2)
	{
		PUTD3DINSTRUCTION(D3DOP_SETSTATUS, sizeof(D3DSTATUS), 1);

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

	// OP_NOP size: 4
	void OP_NOP()
	{
		PUTD3DINSTRUCTION(D3DOP_TRIANGLE, sizeof(D3DTRIANGLE), 0);
	}

private:
	
	void OP_EXIT()
	{
		PUTD3DINSTRUCTION(D3DOP_EXIT, 0, 0);
	}

	bool QWORD_ALIGNED()
	{
		const size_t Start = m_ExecBufferData.size();
		return !(0x00000007L & (ULONG)(Start));
	}
};
