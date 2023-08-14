// D3D Hello World

#pragma once

struct hwd3d_vec3
{
	float x, y, z;

#if defined(__cplusplus)
	hwd3d_vec3(float InX, float InY, float InZ) : x(InX), y(InY), z(InZ) { }
#endif
};

extern const hwd3d_vec3 HWD3DVec3_Zero;
extern const hwd3d_vec3 HWD3DVec3_Ones;

hwd3d_vec3 HWD3DVec3_Add(const hwd3d_vec3& V1, const hwd3d_vec3& V2);
hwd3d_vec3 HWD3DVec3_Subtract(const hwd3d_vec3& V1, const hwd3d_vec3& V2);
float HWD3DVec3_GetMagnitude(const hwd3d_vec3& V);
float HWD3DVec3_GetMagnitudeSq(const hwd3d_vec3& V);
hwd3d_vec3 HWD3DVec3_GetNormalized(const hwd3d_vec3& V);
hwd3d_vec3 HWD3DVec3_GetNegated(const hwd3d_vec3& V);
hwd3d_vec3 HWD3DVec3_Cross(const hwd3d_vec3& V1, const hwd3d_vec3& V2);
float HWD3DVec3_Dot(const hwd3d_vec3& V1, const hwd3d_vec3& V2);

// Meant to be compatible with D3DMATRIX

struct hwd3d_matrix
{
   float _11, _12, _13, _14;
   float _21, _22, _23, _24;
   float _31, _32, _33, _34;
   float _41, _42, _43, _44;
};

extern const hwd3d_matrix HWD3DMatrix_Ident;

hwd3d_matrix HWD3DMatrix_Multiply(const hwd3d_matrix& M1, const hwd3d_matrix& M2);
hwd3d_matrix HWD3DMatrix_BuildPerspectiveLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ);
hwd3d_matrix HWD3DMatrix_BuildPerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ);
hwd3d_matrix HWD3DMatrix_BuildLookAtLH(const hwd3d_vec3& EyePos, const hwd3d_vec3& LookAtPos, const hwd3d_vec3& UpDir);
hwd3d_matrix HWD3DMatrix_BuildTranslation(const hwd3d_vec3& V);
hwd3d_matrix HWD3DMatrix_BuildRotationX(float A);
hwd3d_matrix HWD3DMatrix_BuildRotationY(float A);
hwd3d_matrix HWD3DMatrix_BuildRotationZ(float A);
