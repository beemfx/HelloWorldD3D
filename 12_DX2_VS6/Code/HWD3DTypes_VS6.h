// D3D Hello World

#pragma once

#include "HWD3DSysInclude_VS6.h"

#define HWD3D_SafeRelease(_p) if (_p) { _p->lpVtbl->Release(_p); _p = 0; }
#define HWD3D_Zero(_p) ZeroMemory(_p, sizeof(*_p))

static const float HWD3D_PI_CONST = 3.141592654f;

static float HWD3D_ToRad(float d) { return ((d) * (HWD3D_PI_CONST / 180.0f)); }
static float HWD3D_ToDeg(float r) { return ((r) * (180.0f / HWD3D_PI_CONST)); }

typedef unsigned char hwd3d_byte;

typedef struct _hwd3d_rgba
{
	hwd3d_byte R;
	hwd3d_byte G;
	hwd3d_byte B;
	hwd3d_byte A;
} hwd3d_rgba;

typedef struct _hwd3d_vec2
{
	float x, y;
} hwd3d_vec2;

typedef struct _hwd3d_vec3
{
	float x, y, z;
} hwd3d_vec3;

extern const hwd3d_vec3 HWD3DVec3_Zero;
extern const hwd3d_vec3 HWD3DVec3_Ones;

hwd3d_vec3 HWD3DVec3_Add(const hwd3d_vec3* V1, const hwd3d_vec3* V2);
hwd3d_vec3 HWD3DVec3_Subtract(const hwd3d_vec3* V1, const hwd3d_vec3* V2);
float HWD3DVec3_GetMagnitude(const hwd3d_vec3* V);
float HWD3DVec3_GetMagnitudeSq(const hwd3d_vec3* V);
hwd3d_vec3 HWD3DVec3_GetNormalized(const hwd3d_vec3* V);
hwd3d_vec3 HWD3DVec3_GetNegated(const hwd3d_vec3* V);
hwd3d_vec3 HWD3DVec3_Cross(const hwd3d_vec3* V1, const hwd3d_vec3* V2);
float HWD3DVec3_Dot(const hwd3d_vec3* V1, const hwd3d_vec3* V2);

// Meant to be compatible with D3DMATRIX

typedef struct _hwd3d_matrix
{
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;
} hwd3d_matrix;

extern const hwd3d_matrix HWD3DMatrix_Ident;

hwd3d_matrix HWD3DMatrix_Multiply(const hwd3d_matrix* M1, const hwd3d_matrix* M2);
hwd3d_matrix HWD3DMatrix_BuildPerspectiveLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ);
hwd3d_matrix HWD3DMatrix_BuildPerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ);
hwd3d_matrix HWD3DMatrix_BuildLookAtLH(const hwd3d_vec3* EyePos, const hwd3d_vec3* LookAtPos, const hwd3d_vec3* UpDir);
hwd3d_matrix HWD3DMatrix_BuildTranslation(const hwd3d_vec3* V);
hwd3d_matrix HWD3DMatrix_BuildRotationX(float A);
hwd3d_matrix HWD3DMatrix_BuildRotationY(float A);
hwd3d_matrix HWD3DMatrix_BuildRotationZ(float A);
hwd3d_matrix HWD3DMatrix_BuildScale(const hwd3d_vec3* V);
hwd3d_matrix HWD3DMatrix_Transpose(const hwd3d_matrix* M);

typedef struct _hwd3d_vertex
{
	hwd3d_vec3 Pos;
	hwd3d_vec3 Norm;
	hwd3d_vec2 Tex;
} hwd3d_vertex;

typedef unsigned short hwd3d_graphics_index;

#include <pshpack1.h>
typedef struct _hwd3d_triangle
{
	hwd3d_graphics_index v1;
	hwd3d_graphics_index v2;
	hwd3d_graphics_index v3;
} hwd3d_triangle;
#include <poppack.h>

static_assert( sizeof(hwd3d_triangle) == 3*2 , "Triangle has padding." );
