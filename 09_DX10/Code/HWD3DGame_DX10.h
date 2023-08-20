// D3D Hello World

#include "HWD3DGame.h"
#include <d3d9.h>

class HWD3DGame_DX10 : public HWD3DGame
{
private:
	
	HWND m_TargetWnd = nullptr;
	D3DDEVTYPE m_DevType = D3DDEVTYPE_HAL;
	D3DPRESENT_PARAMETERS m_PP = { };
	IDirect3D9* m_D3D = nullptr;
	IDirect3DTexture9* m_PrimarySurface = nullptr;
	IDirect3DTexture9* m_BackBuffer = nullptr;
	IDirect3DTexture9* m_ZBuffer = nullptr;

	IDirect3DDevice9* m_D3DDevice = nullptr;

	class HWD3DShader_DX10* m_Shader = nullptr;

	hwd3d_matrix m_Proj = HWD3DMatrix_Ident;
	hwd3d_matrix m_View = HWD3DMatrix_Ident;
	hwd3d_matrix m_World = HWD3DMatrix_Ident;
	hwd3d_matrix m_ShaderWVP = HWD3DMatrix_Ident;

public:
	
	IDirect3D9* GetD3D() const { return m_D3D; }
	IDirect3DDevice9* GetDevice() const { return m_D3DDevice; }

private:

	virtual void InitDevice( HWND TargetWnd) override;
	virtual void DeinitDevice() override;

	virtual void ClearViewport() override;
	virtual bool BeginDraw() override;
	virtual void EndDraw() override;
	virtual void Present() override;

	virtual void SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix) override;
};
