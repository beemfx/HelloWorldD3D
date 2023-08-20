// D3D Hello World

#include "HWD3DGame.h"
#include "HWD3DMesh.h"
#include "HWD3DTexture.h"

void HWD3DGame::Release()
{
	Deinit();

	delete this;
}

void HWD3DGame::Init(HWND InMainWnd)
{
	RECT TargetWndRc = {};
	GetClientRect(InMainWnd, &TargetWndRc);
	const int ScreenWidth = TargetWndRc.right - TargetWndRc.left;
	const int ScreenHeight = TargetWndRc.bottom - TargetWndRc.top;
	m_ViewSize = hwd3d_vec2(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));

	InitDevice(InMainWnd);
	CreateScene();
}

void HWD3DGame::Deinit()
{
	DestroyScene();
	DeinitDevice();
}

void HWD3DGame::CreateScene()
{
	m_Teapot.Load(*this, "../_Media/teapot.hw3d", "../_Media/checkers.rgba");
	m_Cube.Load(*this, "../_Media/box.hw3d", "../_Media/HappyFace.rgba");
}

void HWD3DGame::DestroyScene()
{
	m_Cube.Unload();
	m_Teapot.Unload();
}

void HWD3DGame::Update(float DeltaTime)
{
	m_Frame++;

	m_MeshRotationTime += DeltaTime;
	if( m_MeshRotationTime >= m_MeshRotationDuration )
	{
		// We should do something like such as m_MeshRotationTime -= m_MeshRotationDuration,
		// but to keep the code minimal we don't want to deal with situations where the
		// subtraction wouldn't even occur (large float minus small float), so simply reset to 0.
		m_MeshRotationTime = 0.f;
	}

	m_Teapot.UserTranslation = hwd3d_vec3(8.f,-5.f,0.f);

	const float MOVE_SPEED = 50.F;

	if ((GetAsyncKeyState(VK_RIGHT)&(~1)) != 0)
	{
		m_Cube.UserTranslation.x += DeltaTime * MOVE_SPEED;
	}

	if ((GetAsyncKeyState(VK_LEFT)&(~1)) != 0)
	{
		m_Cube.UserTranslation.x -= DeltaTime * MOVE_SPEED;
	}

	if ((GetAsyncKeyState(VK_UP)&(~1)) != 0)
	{
		m_Cube.UserTranslation.y += DeltaTime * MOVE_SPEED;
	}

	if ((GetAsyncKeyState(VK_DOWN)&(~1)) != 0)
	{
		m_Cube.UserTranslation.y -= DeltaTime * MOVE_SPEED;
	}

	// Teapot: Apply rotation, and translate down a bit so the teapot appears centered.
	m_Teapot.WorldMatrix = HWD3DMatrix_Multiply(HWD3DMatrix_BuildRotationY((m_MeshRotationTime/m_MeshRotationDuration) * 2.f * HWD3D_PI_CONST), HWD3DMatrix_BuildTranslation(m_Teapot.UserTranslation));
	// Cube: Scale and translate.
	m_Cube.WorldMatrix = HWD3DMatrix_Multiply(HWD3DMatrix_Multiply(HWD3DMatrix_BuildRotationY(-(m_MeshRotationTime/m_MeshRotationDuration) * 2.f * HWD3D_PI_CONST), HWD3DMatrix_BuildScale(hwd3d_vec3(15.f,15.f,15.f) )), HWD3DMatrix_BuildTranslation(m_Cube.UserTranslation));
}

void HWD3DGame::DrawScene()
{
	ClearViewport();

	if (BeginDraw())
	{
		const hwd3d_matrix ProjMatrix = HWD3DMatrix_BuildPerspectiveFovLH(HWD3D_ToRad(90.f), (static_cast<float>(m_ViewSize.x)/m_ViewSize.y), .1f , 1000.f );
		const hwd3d_matrix ViewMatrix = HWD3DMatrix_BuildLookAtLH(hwd3d_vec3(0.f, 0.f, -25.f), hwd3d_vec3(0.f,0.f,0.f), hwd3d_vec3(0.f,1.f,0.f));

		SetTransformMatrix(hwd3d_transform_t::Proj, ProjMatrix);
		SetTransformMatrix(hwd3d_transform_t::View, ViewMatrix);
		SetTransformMatrix(hwd3d_transform_t::World, HWD3DMatrix_Ident);


		m_Teapot.Draw(*this);
		m_Cube.Draw(*this);

		EndDraw();
	}

	Present();
}

void HWD3DGame::hwd3dMeshSet::Load(HWD3DGame& InGame, const char* InMeshFile, const char* InTextureFile)
{
	Mesh = HWD3DMesh::CreateMesh(&InGame, InMeshFile);
	Texture = HWD3DTexture::CreateTexture(&InGame, InTextureFile);
}

void HWD3DGame::hwd3dMeshSet::Unload()
{
	HWD3D_SafeRelease(Mesh);
	HWD3D_SafeRelease(Texture);
}

void HWD3DGame::hwd3dMeshSet::Draw(HWD3DGame& InGame)
{
	InGame.SetTransformMatrix(HWD3DGame::hwd3d_transform_t::World, WorldMatrix);
	if (Texture)
	{
		Texture->SetTexture();
	}
	if (Mesh)
	{
		Mesh->Draw();
	}
}
