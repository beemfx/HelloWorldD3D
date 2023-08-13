/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: flipcube.c
 *
 *  Mouse controls: Left-click stops cube rotating.  Holding down left mouse
 *     button and moving mouse moves cube.  Right-click resumes rotating.
 *
 ***************************************************************************/

#pragma comment(lib, "ddraw.lib")

#include <windows.h>
#include <windowsx.h>
#include <winnt.h>
#include <math.h>
#include <malloc.h>
#include "d3ddemo.h"

BOOL KeyboardHandler(UINT message, WPARAM wParam, LPARAM lParam);
BOOL MouseHandler(UINT message, WPARAM wParam, LPARAM lParam);
void ConcatenateXRotation(LPD3DMATRIX lpM, float Degrees);
void ConcatenateYRotation(LPD3DMATRIX lpM, float Degrees);
void ConcatenateZRotation(LPD3DMATRIX lpM, float Degrees);

//*** Viewing and perspective
D3DMATRIXHANDLE hProj, hView, hWorld;
D3DMATRIX proj = {
	D3DVAL(2.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(2.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0)
};
D3DMATRIX view = {
	D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(10.0), D3DVAL(1.0)
};
D3DMATRIX identity = {
	D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0)
};
D3DMATRIX world, spin;

//*** Execute buffer
static D3DEXECUTEDATA d3dExData;
static LPDIRECT3DEXECUTEBUFFER lpD3DExBuf;
static D3DEXECUTEBUFFERDESC debDesc;

//*** Interaction
static POINT       Move;
static POINT       Last;

/*
 * A structure which holds the object's data
 */

LPDIRECT3DMATERIAL lpBackgroundMaterial;
/* Cube vertices, normals, shades, and modeling transform */
int NumVertices = 24;
static D3DVERTEX CubeVertices[] = {
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
const int NumTri = 12;
const int CubeTri[] = {
	0, 1, 2, 0, 2, 3,
	4, 5, 6, 4, 6, 7,
	8, 9, 10, 8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23
};

void
OverrideDefaults(Defaults* defaults)
{
	lstrcpy(defaults->Name, "Flipcube D3D Example");
	defaults->rs.ShadeMode = D3DSHADE_GOURAUD;
	defaults->rs.bZBufferOn = TRUE;
	defaults->bTexturesDisabled = FALSE;
}

BOOL
TickScene()
{
	/*
	if (GetAsyncKeyState(VK_LBUTTON) < 0) {
		if (Move.x || Move.y) {
			D3DMATRIX Movement;
			Movement = identity;
			ConcatenateYRotation(&Movement, (float)Move.x);
			ConcatenateXRotation(&Movement, (float)Move.y);
			Move.x = Move.y = 0;
			MultiplyD3DMATRIX(&world, &world, &Movement);
		}
	}
	else {
		MultiplyD3DMATRIX(&world, &spin, &world);
	}
	*/
	world = identity;
	return TRUE;
}

static float GetRawTimeElapsedSec(const LARGE_INTEGER PerfFreq, const LARGE_INTEGER Start, const LARGE_INTEGER End)
{
	const LONGLONG Diff = End.QuadPart - Start.QuadPart;
	const double TimeMs = (Diff * 1000.) / PerfFreq.QuadPart;
	const float TimeSec = ((float)(TimeMs)) / 1000.f;
	return TimeSec;
};

/*
 * Each frame, renders the scene and calls mod_buffer to modify the object
 * for the next frame.
 */
BOOL
RenderScene(LPDIRECT3DDEVICE lpDev, LPDIRECT3DVIEWPORT lpView,
	LPD3DRECT lpExtent)
{
	/*
	 * Execute the instruction buffer
	 */
	if (lpDev->lpVtbl->SetMatrix(lpDev, hWorld, &world) != D3D_OK)
		return FALSE;
	if (lpDev->lpVtbl->BeginScene(lpDev) != D3D_OK)
		return FALSE;
	if (lpDev->lpVtbl->Execute(lpDev, lpD3DExBuf,
		lpView, D3DEXECUTE_CLIPPED) != D3D_OK)
		return FALSE;
	if (lpDev->lpVtbl->EndScene(lpDev) != D3D_OK)
		return FALSE;
	if (lpD3DExBuf->lpVtbl->GetExecuteData(lpD3DExBuf, &d3dExData) != D3D_OK)
		return FALSE;
	*lpExtent = d3dExData.dsStatus.drExtent;

	if (!TickScene())
	{
		return FALSE;
	}

	return TRUE;
}

void
InitSpin(void)
{
	spin = identity;
	ConcatenateYRotation(&spin, D3DVAL(6.0));
	ConcatenateXRotation(&spin, D3DVAL(3.5));
	ConcatenateZRotation(&spin, D3DVAL(2.0));
}

BOOL
InitScene(void)
{
	world = identity;
	InitSpin();
	if (!(SetKeyboardCallback(KeyboardHandler)))
		return FALSE;
	if (!(SetMouseCallback(MouseHandler)))
		return FALSE;
	return TRUE;
}

void
ReleaseScene(void)
{

}

/*
 * Release the memory allocated for the scene and all D3D objects created.
 */
void
ReleaseView(LPDIRECT3DVIEWPORT lpView)
{
	RELEASE(lpD3DExBuf);
	RELEASE(lpBackgroundMaterial);
}

/*
 * Builds the scene and initializes the execute buffer for rendering.  Returns 0 on failure.
 */
BOOL
InitView(LPDIRECTDRAW lpDD, LPDIRECT3D lpD3D, LPDIRECT3DDEVICE lpDev,
	LPDIRECT3DVIEWPORT lpView, int NumTextures,
	LPD3DTEXTUREHANDLE TextureHandle)
{
	D3DMATERIAL MaterialDesc;
	D3DMATERIALHANDLE BackgroundHandle;
	LPVOID lpBufStart, lpInsStart, lpPointer;
	int i;

	/*
	 * Set background to black material
	 */
	if (lpD3D->lpVtbl->CreateMaterial(lpD3D, &lpBackgroundMaterial, NULL) != D3D_OK)
		return FALSE;
	memset(&MaterialDesc, 0, sizeof(D3DMATERIAL));
	MaterialDesc.dwSize = sizeof(D3DMATERIAL);
	MaterialDesc.dwRampSize = 1;
	lpBackgroundMaterial->lpVtbl->SetMaterial(lpBackgroundMaterial, &MaterialDesc);
	lpBackgroundMaterial->lpVtbl->GetHandle(lpBackgroundMaterial, lpDev,
		&BackgroundHandle);
	lpView->lpVtbl->SetBackground(lpView, BackgroundHandle);

	/*
	 * Set the view, world and projection matrices
	 * Create a buffer for matrix set commands etc.
	 */
	MAKE_MATRIX(lpDev, hView, view);
	MAKE_MATRIX(lpDev, hProj, proj);
	MAKE_MATRIX(lpDev, hWorld, world);

	/*
	 * Create an execute buffer
	 */
	// Create an execute buffer
	memset(&debDesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
	debDesc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
	debDesc.dwFlags = D3DDEB_BUFSIZE;
	debDesc.dwBufferSize = 1024*1024;
	if (lpDev->lpVtbl->CreateExecuteBuffer(lpDev, &debDesc, &lpD3DExBuf, NULL)
		!= D3D_OK)
		return FALSE;
	if (lpD3DExBuf->lpVtbl->Lock(lpD3DExBuf, &debDesc) != D3D_OK)
		return FALSE;
	lpBufStart = debDesc.lpData;
	memset(lpBufStart, 0, debDesc.dwBufferSize);
	lpPointer = lpBufStart;

	VERTEX_DATA(&CubeVertices[0], NumVertices, lpPointer);

	lpInsStart = lpPointer;
	OP_SET_STATUS(D3DSETSTATUS_ALL, D3DSTATUS_DEFAULT, 2048, 2048, 0, 0, lpPointer);

	OP_STATE_TRANSFORM(3, lpPointer);
	STATE_DATA(D3DTRANSFORMSTATE_PROJECTION, hProj, lpPointer);
	STATE_DATA(D3DTRANSFORMSTATE_VIEW, hView, lpPointer);
	STATE_DATA(D3DTRANSFORMSTATE_WORLD, hWorld, lpPointer);

	for (i = 0; i < 6; i++) {
		OP_PROCESS_VERTICES(1, lpPointer);
		PROCESSVERTICES_DATA(D3DPROCESSVERTICES_TRANSFORMLIGHT, i * 4, 4, lpPointer);
	}
	OP_STATE_RENDER(3, lpPointer);
	STATE_DATA(D3DRENDERSTATE_TEXTUREHANDLE, TextureHandle[1], lpPointer);
	STATE_DATA(D3DRENDERSTATE_WRAPU, FALSE, lpPointer);
	STATE_DATA(D3DRENDERSTATE_WRAPV, FALSE, lpPointer);
	/*
	 * Make sure that the triangle data (not OP) will be QWORD aligned
	 */
	if (QWORD_ALIGNED(lpPointer)) {
		OP_NOP(lpPointer);
	}
	OP_TRIANGLE_LIST(NumTri, lpPointer);
	for (i = 0; i < NumTri; i++) {
		((LPD3DTRIANGLE)lpPointer)->v1 = CubeTri[i * 3];
		((LPD3DTRIANGLE)lpPointer)->v2 = CubeTri[i * 3 + 1];
		((LPD3DTRIANGLE)lpPointer)->v3 = CubeTri[i * 3 + 2];
		((LPD3DTRIANGLE)lpPointer)->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE;
		((LPD3DTRIANGLE)lpPointer)++;
	}
	OP_EXIT(lpPointer);
	/*
	 * Setup the execute data describing the buffer
	 */
	lpD3DExBuf->lpVtbl->Unlock(lpD3DExBuf);
	memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
	d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
	d3dExData.dwVertexCount = NumVertices;
	d3dExData.dwInstructionOffset = (ULONG)((char*)lpInsStart - (char*)lpBufStart);
	d3dExData.dwInstructionLength = (ULONG)((char*)lpPointer - (char*)lpInsStart);
	lpD3DExBuf->lpVtbl->SetExecuteData(lpD3DExBuf, &d3dExData);

	return TRUE;
}

/****************************************************************************
  Keyboard and mouse handlers
 ****************************************************************************/

BOOL CDECL
KeyboardHandler(UINT message, WPARAM wParam, LPARAM lParam) {
	D3DMATRIX m;
	if (message == WM_KEYDOWN) {
		if ((int)wParam == VK_F11) {
			m = identity;
			m._11 = D3DVAL(1.5);
			m._22 = D3DVAL(1.5);
			m._33 = D3DVAL(1.5);
			MultiplyD3DMATRIX(&world, &world, &m);
			return TRUE;
		}
		else if ((int)wParam == VK_F12) {
			m = identity;
			m._11 = D3DVAL(0.9);
			m._22 = D3DVAL(0.9);
			m._33 = D3DVAL(0.9);
			MultiplyD3DMATRIX(&world, &world, &m);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDECL
MouseHandler(UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_LBUTTONDOWN) {
		/* Get the start location for mouse rotations */
		spin = identity;
		Last.x = LOWORD(lParam);
		Last.y = HIWORD(lParam);
		return TRUE;
	}
	else if (message == WM_RBUTTONDOWN) {
		/* start spinning again */
		InitSpin();
		return TRUE;
	}
	else if (message == WM_MOUSEMOVE) {
		/* While the mouse button is down, keep track of movement
		 * to update the eye position
		 */
		if (GetKeyState(VK_LBUTTON) < 0) {
			Move.x = (int)LOWORD(lParam) - Last.x;
			Move.y = (int)HIWORD(lParam) - Last.y;
			Last.x = LOWORD(lParam);
			Last.y = HIWORD(lParam);
			return TRUE;
		}
	}
	return FALSE;
}

/**************************************************************************
  TransformCube

  Description:
	 Multiplies the world matrix by the given transform matrix
 **************************************************************************/

 /*****************************************************************************
	Rotatations
  *****************************************************************************/
#define M_PI            3.14159265358979323846
void
ConcatenateXRotation(LPD3DMATRIX lpM, float Degrees)
{
	float Temp01, Temp11, Temp21, Temp31;
	float Temp02, Temp12, Temp22, Temp32;
	float aElements[4][4];

	float Radians = (float)((Degrees / 360) * M_PI * 2.0);

	float Sin = (float)sin(Radians), Cos = (float)cos(Radians);

	memcpy(aElements, lpM, sizeof(D3DMATRIX));
	Temp01 = aElements[0][1] * Cos + aElements[0][2] * Sin;
	Temp11 = aElements[1][1] * Cos + aElements[1][2] * Sin;
	Temp21 = aElements[2][1] * Cos + aElements[2][2] * Sin;
	Temp31 = aElements[3][1] * Cos + aElements[3][2] * Sin;

	Temp02 = aElements[0][1] * -Sin + aElements[0][2] * Cos;
	Temp12 = aElements[1][1] * -Sin + aElements[1][2] * Cos;
	Temp22 = aElements[2][1] * -Sin + aElements[2][2] * Cos;
	Temp32 = aElements[3][1] * -Sin + aElements[3][2] * Cos;

	lpM->_12 = Temp01;
	lpM->_22 = Temp11;
	lpM->_32 = Temp21;
	lpM->_42 = Temp31;
	lpM->_13 = Temp02;
	lpM->_23 = Temp12;
	lpM->_33 = Temp22;
	lpM->_43 = Temp32;
}

void
ConcatenateYRotation(LPD3DMATRIX lpM, float Degrees)
{
	float Temp00, Temp10, Temp20, Temp30;
	float Temp02, Temp12, Temp22, Temp32;
	float aElements[4][4];

	float Radians = (float)((Degrees / 360) * M_PI * 2);

	float Sin = (float)sin(Radians), Cos = (float)cos(Radians);

	memcpy(aElements, lpM, sizeof(D3DMATRIX));
	Temp00 = aElements[0][0] * Cos + aElements[0][2] * -Sin;
	Temp10 = aElements[1][0] * Cos + aElements[1][2] * -Sin;
	Temp20 = aElements[2][0] * Cos + aElements[2][2] * -Sin;
	Temp30 = aElements[3][0] * Cos + aElements[3][2] * -Sin;

	Temp02 = aElements[0][0] * Sin + aElements[0][2] * Cos;
	Temp12 = aElements[1][0] * Sin + aElements[1][2] * Cos;
	Temp22 = aElements[2][0] * Sin + aElements[2][2] * Cos;
	Temp32 = aElements[3][0] * Sin + aElements[3][2] * Cos;

	lpM->_11 = Temp00;
	lpM->_21 = Temp10;
	lpM->_31 = Temp20;
	lpM->_41 = Temp30;
	lpM->_13 = Temp02;
	lpM->_23 = Temp12;
	lpM->_33 = Temp22;
	lpM->_43 = Temp32;
}

void
ConcatenateZRotation(LPD3DMATRIX lpM, float Degrees)
{
	float Temp00, Temp10, Temp20, Temp30;
	float Temp01, Temp11, Temp21, Temp31;
	float aElements[4][4];

	float Radians = (float)((Degrees / 360) * M_PI * 2);

	float Sin = (float)sin(Radians), Cos = (float)cos(Radians);

	memcpy(aElements, lpM, sizeof(D3DMATRIX));
	Temp00 = aElements[0][0] * Cos + aElements[0][1] * Sin;
	Temp10 = aElements[1][0] * Cos + aElements[1][1] * Sin;
	Temp20 = aElements[2][0] * Cos + aElements[2][1] * Sin;
	Temp30 = aElements[3][0] * Cos + aElements[3][1] * Sin;

	Temp01 = aElements[0][0] * -Sin + aElements[0][1] * Cos;
	Temp11 = aElements[1][0] * -Sin + aElements[1][1] * Cos;
	Temp21 = aElements[2][0] * -Sin + aElements[2][1] * Cos;
	Temp31 = aElements[3][0] * -Sin + aElements[3][1] * Cos;

	lpM->_11 = Temp00;
	lpM->_21 = Temp10;
	lpM->_31 = Temp20;
	lpM->_41 = Temp30;
	lpM->_12 = Temp01;
	lpM->_22 = Temp11;
	lpM->_32 = Temp21;
	lpM->_42 = Temp31;
}
