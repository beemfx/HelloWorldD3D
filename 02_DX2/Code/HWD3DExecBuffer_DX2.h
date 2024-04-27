// D3D Hello World

#pragma once

#include "HWD3DTypes.h"
#include <d3dtypes.h>

class HWD3DExecBuffer_DX2
{
private:
   
   std::vector<BYTE> m_ExecBuffer;

public:
   
   void PUTD3DINSTRUCTION(BYTE op, BYTE sz, WORD cnt, void* ptr) 
   {
      ((LPD3DINSTRUCTION) ptr)->bOpcode = op;
      ((LPD3DINSTRUCTION) ptr)->bSize = sz;
      ((LPD3DINSTRUCTION) ptr)->wCount = cnt;
      ptr = (void *)(((LPD3DINSTRUCTION) ptr) + 1);
   }

   void VERTEX_DATA(const void* loc, WORD cnt, void* ptr)
   {
       if ((ptr) != (loc)) memcpy((ptr), (loc), sizeof(D3DVERTEX) * (cnt));
       ptr = (void *)(((LPD3DVERTEX) (ptr)) + (cnt));
   }

   // OP_MATRIX_MULTIPLY size: 4 (sizeof D3DINSTRUCTION)
   void OP_MATRIX_MULTIPLY(WORD cnt, void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_MATRIXMULTIPLY, sizeof(D3DMATRIXMULTIPLY), cnt, ptr);
   }

   // MATRIX_MULTIPLY_DATA size: 12 (sizeof MATRIXMULTIPLY)
   void MATRIX_MULTIPLY_DATA(D3DMATRIXHANDLE src1, D3DMATRIXHANDLE src2, D3DMATRIXHANDLE dest, void* ptr)
   {
       ((LPD3DMATRIXMULTIPLY) ptr)->hSrcMatrix1 = src1;
       ((LPD3DMATRIXMULTIPLY) ptr)->hSrcMatrix2 = src2;
       ((LPD3DMATRIXMULTIPLY) ptr)->hDestMatrix = dest;
       ptr = (void *)(((LPD3DMATRIXMULTIPLY) ptr) + 1);
   }

   // OP_STATE_LIGHT size: 4 (sizeof D3DINSTRUCTION)
   void OP_STATE_LIGHT(WORD cnt, void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_STATELIGHT, sizeof(D3DSTATE), cnt, ptr);
   }

   // OP_STATE_TRANSFORM size: 4 (sizeof D3DINSTRUCTION)
   void OP_STATE_TRANSFORM(WORD cnt, void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_STATETRANSFORM, sizeof(D3DSTATE), cnt, ptr);
   }

   // OP_STATE_RENDER size: 4 (sizeof D3DINSTRUCTION)
   void OP_STATE_RENDER(WORD cnt, void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_STATERENDER, sizeof(D3DSTATE), cnt, ptr);
   }

   // STATE_DATA size: 8 (sizeof D3DSTATE)
   void STATE_DATA(D3DRENDERSTATETYPE type, DWORD arg, void* ptr)
   {
       ((LPD3DSTATE) ptr)->drstRenderStateType = (D3DRENDERSTATETYPE)type;
       ((LPD3DSTATE) ptr)->dwArg[0] = arg;
       ptr = (void *)(((LPD3DSTATE) ptr) + 1);
   }

   // OP_PROCESS_VERTICES size: 4 (sizeof D3DINSTRUCTION)
   void OP_PROCESS_VERTICES(WORD cnt, void* ptr)
   {
    PUTD3DINSTRUCTION(D3DOP_PROCESSVERTICES, sizeof(D3DPROCESSVERTICES), cnt, ptr);
    }

   // PROCESSVERTICES_DATA size: 16 (sizeof D3DPROCESSVERTICES)
   void PROCESSVERTICES_DATA(DWORD flgs, WORD strt, WORD cnt, void* ptr)
   {
       ((LPD3DPROCESSVERTICES) ptr)->dwFlags = flgs;
       ((LPD3DPROCESSVERTICES) ptr)->wStart = strt;
       ((LPD3DPROCESSVERTICES) ptr)->wDest = strt;
       ((LPD3DPROCESSVERTICES) ptr)->dwCount = cnt;
       ((LPD3DPROCESSVERTICES) ptr)->dwReserved = 0;
       ptr = (void *)(((LPD3DPROCESSVERTICES) ptr) + 1);
   }

   // OP_TRIANGLE_LIST size: 4 (sizeof D3DINSTRUCTION)
   void OP_TRIANGLE_LIST(WORD cnt, void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_TRIANGLE, sizeof(D3DTRIANGLE), cnt, ptr);
   }

   void TRIANGLE_LIST_DATA(const void* loc, DWORD count, void* ptr)
   {
       if ((ptr) != (loc)) memcpy((ptr), (loc), sizeof(D3DTRIANGLE) * (count));
       ptr = (void *)(((LPD3DTRIANGLE) (ptr)) + (count));
   }

   // OP_LINE_LIST size: 4 (sizeof D3DINSTRUCTION)
   void OP_LINE_LIST(WORD cnt, void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_LINE, sizeof(D3DLINE), cnt, ptr);
   }

   void LINE_LIST_DATA(const void* loc, WORD count, void* ptr)
   {
       if ((ptr) != (loc)) memcpy((ptr), (loc), sizeof(D3DLINE) * (count));
       ptr = (void *)(((LPD3DLINE) (ptr)) + (count));
   }

   // OP_POINT_LIST size: 8 (sizeof D3DINSTRUCTION + sizeof D3DPOINT)
   void OP_POINT_LIST(WORD first, WORD cnt, void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_POINT, sizeof(D3DPOINT), 1, ptr);
       ((LPD3DPOINT)(ptr))->wCount = cnt;
       ((LPD3DPOINT)(ptr))->wFirst = first;
       ptr = (void*)(((LPD3DPOINT)(ptr)) + 1);
   }

   // OP_SPAN_LIST size: 8 (sizeof D3DINSTRUCTION + sizeof D3DSPAN)
void OP_SPAN_LIST(WORD first, WORD cnt, void* ptr)
{
    PUTD3DINSTRUCTION(D3DOP_SPAN, sizeof(D3DSPAN), 1, ptr);
    ((LPD3DSPAN)(ptr))->wCount = cnt;
    ((LPD3DSPAN)(ptr))->wFirst = first;
    ptr = (void*)(((LPD3DSPAN)(ptr)) + 1);
}

   // OP_BRANCH_FORWARD size: 18 (sizeof D3DINSTRUCTION + sizeof D3DBRANCH)
   void OP_BRANCH_FORWARD(DWORD tmask, DWORD tvalue, BOOL tnegate, DWORD toffset, void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_BRANCHFORWARD, sizeof(D3DBRANCH), 1, ptr);
       ((LPD3DBRANCH) ptr)->dwMask = tmask;
       ((LPD3DBRANCH) ptr)->dwValue = tvalue;
       ((LPD3DBRANCH) ptr)->bNegate = tnegate;
       ((LPD3DBRANCH) ptr)->dwOffset = toffset;
       ptr = (void *)(((LPD3DBRANCH) (ptr)) + 1);
   }

   // OP_SET_STATUS size: 20 (sizeof D3DINSTRUCTION + sizeof D3DSTATUS)
   void OP_SET_STATUS(DWORD flags, DWORD status, LONG _x1, LONG _y1, LONG _x2, LONG _y2, void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_SETSTATUS, sizeof(D3DSTATUS), 1, ptr);
       ((LPD3DSTATUS)(ptr))->dwFlags = flags;
       ((LPD3DSTATUS)(ptr))->dwStatus = status;
       ((LPD3DSTATUS)(ptr))->drExtent.x1 = _x1;
       ((LPD3DSTATUS)(ptr))->drExtent.y1 = _y1;
       ((LPD3DSTATUS)(ptr))->drExtent.x2 = _x2;
       ((LPD3DSTATUS)(ptr))->drExtent.y2 = _y2;
       ptr = (void *)(((LPD3DSTATUS) (ptr)) + 1);
   }

   // OP_NOP size: 4
   void OP_NOP(void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_TRIANGLE, sizeof(D3DTRIANGLE), 0, ptr);
   }

   void OP_EXIT(void* ptr)
   {
       PUTD3DINSTRUCTION(D3DOP_EXIT, 0, 0, ptr);
   }

   static bool QWORD_ALIGNED(void* ptr)
   {
      return !(0x00000007L & (ULONG)(ptr));
   }
};
