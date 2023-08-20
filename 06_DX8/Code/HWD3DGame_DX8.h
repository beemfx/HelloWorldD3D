// D3D Hello World

#include "HWD3DGame.h"
#include <d3d8.h>

class HWD3DGame_DX8 : public HWD3DGame
{
private:
	
	HWND m_TargetWnd = nullptr;
	D3DDEVTYPE m_DevType = D3DDEVTYPE_HAL;
	D3DPRESENT_PARAMETERS m_PP = { };
	IDirect3D8* m_D3D = nullptr;
	IDirect3DTexture8* m_PrimarySurface = nullptr;
	IDirect3DTexture8* m_BackBuffer = nullptr;
	IDirect3DTexture8* m_ZBuffer = nullptr;

	IDirect3DDevice8* m_D3DDevice = nullptr;

public:
	
	IDirect3D8* GetD3D() const { return m_D3D; }
	IDirect3DDevice8* GetDevice() const { return m_D3DDevice; }

private:

	virtual void InitDevice( HWND TargetWnd) override;
	virtual void DeinitDevice() override;

	virtual void ClearViewport() override;
	virtual bool BeginDraw() override;
	virtual void EndDraw() override;
	virtual void Present() override;

	virtual void SetWorldMatrix(const hwd3d_matrix& InMatrix) override;

	void InitCommonStates();
	void ResetSceneStates();
};
