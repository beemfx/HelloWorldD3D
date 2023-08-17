// D3D Hello World

#include "HWD3DTypes.h"
#include <math.h>

const hwd3d_vec3 HWD3DVec3_Zero = { 0.f , 0.f , 0.f };
const hwd3d_vec3 HWD3DVec3_Ones = { 1.f , 1.f , 1.f };

hwd3d_vec3 HWD3DVec3_Add(const hwd3d_vec3& V1, const hwd3d_vec3& V2)
{
	const hwd3d_vec3 Out = { V1.x + V2.x , V1.y + V2.y , V1.z + V2.z };
	return Out;
}

hwd3d_vec3 HWD3DVec3_Subtract(const hwd3d_vec3& V1, const hwd3d_vec3& V2)
{
	const hwd3d_vec3 Out = { V1.x - V2.x , V1.y - V2.y , V1.z - V2.z };
	return Out;
}

float HWD3DVec3_GetMagnitude(const hwd3d_vec3& V)
{
	return sqrtf(HWD3DVec3_GetMagnitudeSq(V));
}

float HWD3DVec3_GetMagnitudeSq(const hwd3d_vec3& V)
{
	return HWD3DVec3_Dot(V, V);
}

hwd3d_vec3 HWD3DVec3_GetNormalized(const hwd3d_vec3& V)
{
	const float Mag = HWD3DVec3_GetMagnitude(V);
	const hwd3d_vec3 Out = { V.x/Mag , V.y/Mag , V.z/Mag };
	return Out;
}

hwd3d_vec3 HWD3DVec3_GetNegated(const hwd3d_vec3& V)
{
	const hwd3d_vec3 Out = { -V.x , -V.y , -V.z };
	return Out;
}

hwd3d_vec3 HWD3DVec3_Cross(const hwd3d_vec3& V1, const hwd3d_vec3& V2)
{
	const hwd3d_vec3 Out = { V1.y * V2.z - V2.y * V1.z , V2.x * V1.z - V1.x * V2.z , V1.x * V2.y - V2.x * V1.y };
	return Out;
}

float HWD3DVec3_Dot(const hwd3d_vec3& V1, const hwd3d_vec3& V2)
{
	return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z;
}

const hwd3d_matrix HWD3DMatrix_Ident =
{
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f,
};

hwd3d_matrix HWD3DMatrix_Multiply(const hwd3d_matrix& M1, const hwd3d_matrix& M2)
{
	hwd3d_matrix Out;

	// Cache the invariants in registers
	float x = M1._11;
	float y = M1._12;
	float z = M1._13;
	float w = M1._14;

	// Perform the operation on the first row
	Out._11 = (M2._11 * x) + (M2._21 * y) + (M2._31 * z) + (M2._41 * w);
	Out._12 = (M2._12 * x) + (M2._22 * y) + (M2._32 * z) + (M2._42 * w);
	Out._13 = (M2._13 * x) + (M2._23 * y) + (M2._33 * z) + (M2._43 * w);
	Out._14 = (M2._14 * x) + (M2._24 * y) + (M2._34 * z) + (M2._44 * w);
	// Repeat for all the other rows
	x = M1._21;
	y = M1._22;
	z = M1._23;
	w = M1._24;
	Out._21 = (M2._11 * x) + (M2._21 * y) + (M2._31 * z) + (M2._41 * w);
	Out._22 = (M2._12 * x) + (M2._22 * y) + (M2._32 * z) + (M2._42 * w);
	Out._23 = (M2._13 * x) + (M2._23 * y) + (M2._33 * z) + (M2._43 * w);
	Out._24 = (M2._14 * x) + (M2._24 * y) + (M2._34 * z) + (M2._44 * w);
	x = M1._31;
	y = M1._32;
	z = M1._33;
	w = M1._34;
	Out._31 = (M2._11 * x) + (M2._21 * y) + (M2._31 * z) + (M2._41 * w);
	Out._32 = (M2._12 * x) + (M2._22 * y) + (M2._32 * z) + (M2._42 * w);
	Out._33 = (M2._13 * x) + (M2._23 * y) + (M2._33 * z) + (M2._43 * w);
	Out._34 = (M2._14 * x) + (M2._24 * y) + (M2._34 * z) + (M2._44 * w);
	x = M1._41;
	y = M1._42;
	z = M1._43;
	w = M1._44;
	Out._41 = (M2._11 * x) + (M2._21 * y) + (M2._31 * z) + (M2._41 * w);
	Out._42 = (M2._12 * x) + (M2._22 * y) + (M2._32 * z) + (M2._42 * w);
	Out._43 = (M2._13 * x) + (M2._23 * y) + (M2._33 * z) + (M2._43 * w);
	Out._44 = (M2._14 * x) + (M2._24 * y) + (M2._34 * z) + (M2._44 * w);
	return Out;
}

hwd3d_matrix HWD3DMatrix_BuildPerspectiveLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
{
	const float TwoNearZ = NearZ + NearZ;
	const float fRange = FarZ / (FarZ - NearZ);

	hwd3d_matrix M;
	M._11 = TwoNearZ / ViewWidth;
	M._12 = 0.0f;
	M._13 = 0.0f;
	M._14 = 0.0f;

	M._21 = 0.0f;
	M._22 = TwoNearZ / ViewHeight;
	M._23 = 0.0f;
	M._24 = 0.0f;

	M._31 = 0.0f;
	M._32 = 0.0f;
	M._33 = fRange;
	M._34 = 1.0f;

	M._41 = 0.0f;
	M._42 = 0.0f;
	M._43 = -fRange * NearZ;
	M._44 = 0.0f;
	return M;
}

hwd3d_matrix HWD3DMatrix_BuildPerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
{
	const float SinFov = sinf(0.5f * FovAngleY);
	const float CosFov = cosf(0.5f * FovAngleY);
	const float Height = CosFov / SinFov;
	const float Width = Height / AspectRatio;
	const float fRange = FarZ / (FarZ - NearZ);

	hwd3d_matrix M;
	M._11 = Width;
	M._12 = 0.0f;
	M._13 = 0.0f;
	M._14 = 0.0f;

	M._21 = 0.0f;
	M._22 = Height;
	M._23 = 0.0f;
	M._24 = 0.0f;

	M._31 = 0.0f;
	M._32 = 0.0f;
	M._33 = fRange;
	M._34 = 1.0f;

	M._41 = 0.0f;
	M._42 = 0.0f;
	M._43 = -fRange * NearZ;
	M._44 = 0.0f;
	return M;
}

hwd3d_matrix HWD3DMatrix_BuildLookAtLH(const hwd3d_vec3& EyePos, const hwd3d_vec3& LookAtPos, const hwd3d_vec3& UpDir)
{
	const hwd3d_vec3 ZAxis = HWD3DVec3_GetNormalized(HWD3DVec3_Subtract(LookAtPos, EyePos));
	const hwd3d_vec3 XAxis = HWD3DVec3_GetNormalized(HWD3DVec3_Cross(UpDir, ZAxis));
	const hwd3d_vec3 YAxis = HWD3DVec3_Cross(ZAxis, XAxis);

	const float DX = -HWD3DVec3_Dot(XAxis, EyePos);
	const float DY = -HWD3DVec3_Dot(YAxis, EyePos);
	const float DZ = -HWD3DVec3_Dot(ZAxis, EyePos);

	const hwd3d_matrix M =
	{
		XAxis.x, YAxis.x, ZAxis.x, 0.f,
		XAxis.y, YAxis.y, ZAxis.y, 0.f,
		XAxis.z, YAxis.z, ZAxis.z, 0.f,
		DX, DY, DZ, 1.f,
	};
	return M;
}

hwd3d_matrix HWD3DMatrix_BuildTranslation(const hwd3d_vec3& V)
{
	const hwd3d_matrix M =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		V.x, V.y, V.z, 1.f,
	};
	return M;
}

hwd3d_matrix HWD3DMatrix_BuildRotationX(float A)
{
	const float SinA = sinf(A);
	const float CosA = cosf(A);

	const hwd3d_matrix M =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, CosA, -SinA, 0.f,
		0.f, SinA, CosA, 0.f,
		0.f, 0.f, 0.f, 1.f,
	};
	return M;
}

hwd3d_matrix HWD3DMatrix_BuildRotationY(float A)
{
	const float SinA = sinf(A);
	const float CosA = cosf(A);

	const hwd3d_matrix M =
	{
		CosA, 0.f, SinA, 0.f,
		0.f, 1.f, 0.f, 0.f,
		-SinA, 0.f, CosA, 0.f,
		0.f, 0.f, 0.f, 1.f,
	};
	return M;
}

hwd3d_matrix HWD3DMatrix_BuildRotationZ(float A)
{
	const float SinA = sinf(A);
	const float CosA = cosf(A);

	const hwd3d_matrix M =
	{
		CosA, -SinA, 0.f, 0.f,
		SinA, CosA, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f,
	};
	return M;
}
