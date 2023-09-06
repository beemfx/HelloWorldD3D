// D3D Hello World

#pragma once

#include "HWD3DTypes_VS6.h"

typedef struct _hwd3dMeshSet
{
	struct HWD3DMesh_VS6* Mesh;
	struct HWD3DTexture_VS6* Texture;
	hwd3d_matrix WorldMatrix;
	hwd3d_vec3 UserTranslation;
} hwd3dMeshSet;

typedef struct _HWD3DGame_VS6
{
	hwd3d_vec2 ViewSize;
	int Frame;

	hwd3dMeshSet Teapot;
	hwd3dMeshSet Cube;

	float MeshRotationTime;
	float MeshRotationDuration;
} HWD3DGame_VS6;

HWD3DGame_VS6* HWD3DGame_VS6_CreateGame(HWND InMainWnd);
void HWD3DGame_VS6_DestroyGame(HWD3DGame_VS6* InGame);
void HWD3DGame_VS6_Update(HWD3DGame_VS6* InGame, float DeltaTime);
void HWD3DGame_VS6_DrawScene(HWD3DGame_VS6* InGame);

