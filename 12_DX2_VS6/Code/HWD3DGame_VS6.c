// D3D Hello World

#include "HWD3DGame_VS6.h"
// #include "HWD3DMesh_VS6.h"
// #include "HWD3DTexture_VS6.h"

typedef enum _hwd3d_transform_t
{
	hwd3d_transform_t_Proj ,
	hwd3d_transform_t_View ,
	hwd3d_transform_t_World ,
} hwd3d_transform_t;

// HWD3DGame_VS6 Private Interface

static void HWD3DGame_VS6_CreateScene(HWD3DGame_VS6* _this);
static void HWD3DGame_VS6_DestroyScene(HWD3DGame_VS6* _this);

static void HWD3DGame_VS6_InitDevice(HWD3DGame_VS6* _this, HWND InMainWnd) { }
static void HWD3DGame_VS6_DeinitDevice(HWD3DGame_VS6* _this) { }

static void HWD3DGame_VS6_ClearViewport(HWD3DGame_VS6* _this) { }
static BOOL HWD3DGame_VS6_BeginDraw(HWD3DGame_VS6* _this) { return TRUE; }
static void HWD3DGame_VS6_EndDraw(HWD3DGame_VS6* _this) { }
static void HWD3DGame_VS6_Present(HWD3DGame_VS6* _this) { }

static void HWD3DGame_VS6_SetTransformMatrix(HWD3DGame_VS6* _this, hwd3d_transform_t InType, const hwd3d_matrix* InMatrix) { }

static HWD3DGame_VS6_Init(HWD3DGame_VS6* _this, HWND InMainWnd);
static HWD3DGame_VS6_Deinit(HWD3DGame_VS6* _this);

// hwd3dMeshSet Private Interface

static void hwd3dMeshSet_Load(hwd3dMeshSet* MeshSet, HWD3DGame_VS6* InGame, const char* InMeshFile, const char* InTextureFile);
static void hwd3dMeshSet_Unload(hwd3dMeshSet* MeshSet);
static void hwd3dMeshSet_Draw(hwd3dMeshSet* MeshSet, HWD3DGame_VS6* InGame);

HWD3DGame_VS6* HWD3DGame_VS6_CreateGame(HWND InMainWnd)
{
	HWD3DGame_VS6* Out = malloc(sizeof(HWD3DGame_VS6));
	if (!Out)
	{
		return NULL;
	}

	HWD3D_Zero(Out);

	RECT TargetWndRc;
	HWD3D_Zero(&TargetWndRc);
	GetClientRect(InMainWnd, &TargetWndRc);
	const int ScreenWidth = TargetWndRc.right - TargetWndRc.left;
	const int ScreenHeight = TargetWndRc.bottom - TargetWndRc.top;

	Out->ViewSize.x = (float)ScreenWidth;
	Out->ViewSize.y = (float)ScreenHeight;

	Out->MeshRotationDuration = 5.F;

	HWD3DGame_VS6_InitDevice(Out, InMainWnd);
	HWD3DGame_VS6_CreateScene(Out);

	return Out;
}

void HWD3DGame_VS6_DestroyGame(HWD3DGame_VS6* InGame)
{
	HWD3DGame_VS6_DestroyScene(InGame);
	HWD3DGame_VS6_DeinitDevice(InGame);
	free(InGame);
}

static void HWD3DGame_VS6_CreateScene(HWD3DGame_VS6* _this)
{
	hwd3dMeshSet_Load(&_this->Teapot, _this, "_Media/teapot.hw3d", "_Media/checkers.rgba");
	hwd3dMeshSet_Load(&_this->Cube, _this, "_Media/box.hw3d", "_Media/HappyFace.rgba");
}

static void HWD3DGame_VS6_DestroyScene(HWD3DGame_VS6* _this)
{
	hwd3dMeshSet_Unload(&_this->Cube);
	hwd3dMeshSet_Unload(&_this->Teapot);
}

void HWD3DGame_VS6_Update(HWD3DGame_VS6* InGame, float DeltaTime)
{
	InGame->Frame++;

	InGame->MeshRotationTime += DeltaTime;
	if( InGame->MeshRotationTime >= InGame->MeshRotationDuration )
	{
		// We should do something like such as m_MeshRotationTime -= m_MeshRotationDuration,
		// but to keep the code minimal we don't want to deal with situations where the
		// subtraction wouldn't even occur (large float minus small float), so simply reset to 0.
		InGame->MeshRotationTime = 0.f;
	}

	InGame->Teapot.UserTranslation.x = 8.f;
	InGame->Teapot.UserTranslation.y = -5.f;
	InGame->Teapot.UserTranslation.z = 0.f;

	const float MOVE_SPEED = 50.F;

	if ((GetAsyncKeyState(VK_RIGHT)&(~1)) != 0)
	{
		InGame->Cube.UserTranslation.x += DeltaTime * MOVE_SPEED;
	}

	if ((GetAsyncKeyState(VK_LEFT)&(~1)) != 0)
	{
		InGame->Cube.UserTranslation.x -= DeltaTime * MOVE_SPEED;
	}

	if ((GetAsyncKeyState(VK_UP)&(~1)) != 0)
	{
		InGame->Cube.UserTranslation.y += DeltaTime * MOVE_SPEED;
	}

	if ((GetAsyncKeyState(VK_DOWN)&(~1)) != 0)
	{
		InGame->Cube.UserTranslation.y -= DeltaTime * MOVE_SPEED;
	}

	// Teapot: Apply rotation, and translate down a bit so the teapot appears centered.
	{
		const hwd3d_matrix RotY = HWD3DMatrix_BuildRotationY((InGame->MeshRotationTime/InGame->MeshRotationDuration) * 2.f * HWD3D_PI_CONST);
		const hwd3d_matrix Trans = HWD3DMatrix_BuildTranslation(&InGame->Teapot.UserTranslation);
		InGame->Teapot.WorldMatrix = HWD3DMatrix_Multiply(&RotY, &Trans);
	}
	// Cube: Scale and translate.
	{
		const hwd3d_matrix RotY = HWD3DMatrix_BuildRotationY(-(InGame->MeshRotationTime/InGame->MeshRotationDuration) * 2.f * HWD3D_PI_CONST);
		const hwd3d_vec3 ScaleVec = { 15.f , 15.f , 15.f };
		const hwd3d_matrix Scale = HWD3DMatrix_BuildScale(&ScaleVec);
		const hwd3d_matrix Trans = HWD3DMatrix_BuildTranslation(&InGame->Cube.UserTranslation);
		const hwd3d_matrix RotAndScale = HWD3DMatrix_Multiply(&RotY, &Scale);
		InGame->Cube.WorldMatrix = HWD3DMatrix_Multiply(&RotAndScale, &Trans);
	}
}

void HWD3DGame_VS6_DrawScene(HWD3DGame_VS6* _this)
{
	HWD3DGame_VS6_ClearViewport(_this);

	if (HWD3DGame_VS6_BeginDraw(_this))
	{
		// Set transforms:
		{
			const hwd3d_matrix ProjMatrix = HWD3DMatrix_BuildPerspectiveFovLH(HWD3D_ToRad(90.f), ((float)(_this->ViewSize.x)/_this->ViewSize.y), .1f , 1000.f );
			const hwd3d_vec3 LookEye = { 0.f , 0.f , -25.f };
			const hwd3d_vec3 LookAt = { 0.f , 0.f , 0.f };
			const hwd3d_vec3 LookUp = { 0.f , 1.f , 0.f };
			const hwd3d_matrix ViewMatrix = HWD3DMatrix_BuildLookAtLH(&LookEye, &LookAt, &LookUp);

			HWD3DGame_VS6_SetTransformMatrix(_this, hwd3d_transform_t_Proj, &ProjMatrix);
			HWD3DGame_VS6_SetTransformMatrix(_this, hwd3d_transform_t_View, &ViewMatrix);
			HWD3DGame_VS6_SetTransformMatrix(_this, hwd3d_transform_t_World, &HWD3DMatrix_Ident);
		}

		hwd3dMeshSet_Draw(&_this->Teapot, _this);
		hwd3dMeshSet_Draw(&_this->Cube, _this);

		HWD3DGame_VS6_EndDraw(_this);
	}

	HWD3DGame_VS6_Present(_this);
}

static void hwd3dMeshSet_Load(hwd3dMeshSet* _this, HWD3DGame_VS6* InGame, const char* InMeshFile, const char* InTextureFile)
{
	_this->WorldMatrix = HWD3DMatrix_Ident;
	_this->UserTranslation.x = 0.f;
	_this->UserTranslation.y = 0.f;
	_this->UserTranslation.z = 0.f;
	_this->Mesh = NULL;
	_this->Texture = NULL;
	// Mesh = HWD3DMesh::CreateMesh(&InGame, InMeshFile);
	// Texture = HWD3DTexture::CreateTexture(&InGame, InTextureFile);
}

static void hwd3dMeshSet_Unload(hwd3dMeshSet* _this)
{
	// HWD3D_SafeRelease(_this->Mesh);
	// HWD3D_SafeRelease(_this->Texture);
}

static void hwd3dMeshSet_Draw(hwd3dMeshSet* _this, HWD3DGame_VS6* InGame)
{
	HWD3DGame_VS6_SetTransformMatrix(InGame, hwd3d_transform_t_World, &_this->WorldMatrix);
	if (_this->Texture)
	{
		// _this->Texture->SetTexture();
	}
	if (_this->Mesh)
	{
		// _this->Mesh->Draw();
	}
}
