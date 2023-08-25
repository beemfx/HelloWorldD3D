// D3D Hello World

#include "HWD3DGame_DX9_SM3.h"
#include "HWD3DShader_DX9_SM3.h"

#pragma comment(lib, "d3d9.lib")

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	SetWindowTextW(InMainWnd, L"Hello World D3D [DX9 SM3]");

	HWD3DGame* Out = new HWD3DGame_DX9_SM3;
	if (Out)
	{
		Out->Init(InMainWnd);
	}

	return Out;
}

void HWD3DGame_DX9_SM3::InitDevice(HWND TargetWnd)
{
	m_TargetWnd = TargetWnd;

	RECT TargetWndRc = {};
	GetClientRect(m_TargetWnd, &TargetWndRc);
	const int ScreenWidth = TargetWndRc.right - TargetWndRc.left;
	const int ScreenHeight = TargetWndRc.bottom - TargetWndRc.top;

	// Init Device
	{
		m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
		if (!m_D3D)
		{
			Deinit();
			return;
		}

		m_DevType = D3DDEVTYPE_HAL;

		m_PP.BackBufferWidth = ScreenWidth;
		m_PP.BackBufferHeight = ScreenHeight;
		m_PP.BackBufferCount = 1;
		m_PP.BackBufferFormat = D3DFMT_X8R8G8B8; // Same as desktop since we're windowed.
		m_PP.EnableAutoDepthStencil = TRUE;
		m_PP.AutoDepthStencilFormat = D3DFMT_D16;
		m_PP.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_PP.Windowed = TRUE;
		m_PP.hDeviceWindow = m_TargetWnd;

		const HRESULT CreateHalRes = m_D3D->CreateDevice(D3DADAPTER_DEFAULT, m_DevType, m_TargetWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_PP, &m_D3DDevice);
		if (FAILED(CreateHalRes) || !m_D3DDevice)
		{
			m_DevType = D3DDEVTYPE_REF;
			const HRESULT CreateRefRes = m_D3D->CreateDevice(D3DADAPTER_DEFAULT, m_DevType, m_TargetWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_PP, &m_D3DDevice);
			if (FAILED(CreateRefRes) || !m_D3DDevice)
			{
				Deinit();
				return;
			}
		}
	}

	m_Shader = HWD3DShader_DX9_SM3::CreateShader(this, "_Media/DX9_SM3_VS.cso", "_Media/DX9_SM3_PS.cso");
}

void HWD3DGame_DX9_SM3::DeinitDevice()
{
	HWD3D_SafeRelease(m_Shader);
	HWD3D_SafeRelease(m_D3DDevice);
	HWD3D_SafeRelease(m_ZBuffer);
	HWD3D_SafeRelease(m_BackBuffer);
	HWD3D_SafeRelease(m_PrimarySurface);
	HWD3D_SafeRelease(m_D3D);
}

void HWD3DGame_DX9_SM3::ClearViewport()
{
	if (m_D3DDevice)
	{
		const HRESULT Res = m_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFF6666FF, 1.f, 0);
		assert(SUCCEEDED(Res));
	}
}

bool HWD3DGame_DX9_SM3::BeginDraw()
{
	if (m_D3DDevice)
	{
		if (SUCCEEDED(m_D3DDevice->BeginScene()))
		{
			if (m_Shader)
			{
				m_Shader->SetShader();
			}

			return true;
		}
	}

	return false;
}

void HWD3DGame_DX9_SM3::EndDraw()
{
	if (m_D3DDevice)
	{
		const HRESULT EndSceneRes = m_D3DDevice->EndScene();
		assert(SUCCEEDED(EndSceneRes));
	}
}

void HWD3DGame_DX9_SM3::Present()
{
	if (m_D3DDevice)
	{
		m_D3DDevice->Present(NULL, NULL, NULL, NULL);
	}
}

void HWD3DGame_DX9_SM3::SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix)
{
	if (m_D3DDevice)
	{
		hwd3d_matrix* Mat = nullptr;

		switch (InType)
		{
		case HWD3DGame::hwd3d_transform_t::Proj:
			Mat = &m_Proj;
			break;
		case HWD3DGame::hwd3d_transform_t::View:
			Mat = &m_View;
			break;
		case HWD3DGame::hwd3d_transform_t::World:
			Mat = &m_World;
			break;
		}

		if (Mat)
		{
			*Mat = InMatrix;

			m_ShaderWVP = HWD3DMatrix_Transpose(HWD3DMatrix_Multiply(m_World, HWD3DMatrix_Multiply(m_View, m_Proj)));

			// Optimally we wouldn't update the constant buffer every time a matrix changed, just before we do a draw, 
			// but for Hello World sample this is acceptable, with the understanding that it should be optimized.
			// We do have a slight optimization in that in this program we know that setting the world matrix
			// indicates that a draw is a bout to happen.
			const bool bShouldUpdateCosntantBuffer = InType == hwd3d_transform_t::World;
			if (bShouldUpdateCosntantBuffer)
			{
				m_D3DDevice->SetVertexShaderConstantF(0, reinterpret_cast<const float*>(&m_ShaderWVP), 4);
			}
		}
	}
}
