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

}

void HWD3DGame::Deinit()
{

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

	// Translate down a bit so the teapot appears centered.
	m_MeshMatrix = HWD3DMatrix_Multiply(HWD3DMatrix_BuildTranslation(hwd3d_vec3(0.f, -5.f, 0.f) ), HWD3DMatrix_BuildRotationY((m_MeshRotationTime/m_MeshRotationDuration) * 2.f * HWD3D_PI_CONST));
}

void HWD3DGame::DrawScene()
{
	ClearViewport();

	if (BeginDraw())
	{
		if (m_Mesh)
		{
			SetWorldMatrix(m_MeshMatrix);
			if (m_Texture)
			{
				m_Texture->SetTexture();
			}
			m_Mesh->Draw();
		}

		EndDraw();
	}

	Present();
}

void HWD3DGame::ClearViewport()
{

}

bool HWD3DGame::BeginDraw()
{
	return true;
}

void HWD3DGame::EndDraw()
{

}

void HWD3DGame::Present()
{

}

void HWD3DGame::SetWorldMatrix(const hwd3d_matrix& InMatrix)
{

}
