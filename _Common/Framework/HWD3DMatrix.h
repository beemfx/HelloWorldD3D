// D3D Hello World

#pragma once

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
