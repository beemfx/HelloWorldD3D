// D3D Hello World

#include "HWD3DGame.h"
#include <d3d11.h>

class HWD3DGame_DX11 : public HWD3DGame
{
private:
	
	HWND m_TargetWnd = nullptr;
	UINT m_ViewWidth = 0;
	UINT m_ViewHeight = 0;

	IDXGISwapChain* m_SwapChain = nullptr;
	ID3D11Device* m_D3DDevice = nullptr;
	ID3D11DeviceContext* m_D3DContext = nullptr;
	ID3D11Texture2D* m_RTVTexture = nullptr;
	ID3D11RenderTargetView* m_RTV = nullptr;
	ID3D11Texture2D* m_DSVTexture = nullptr;
	ID3D11DepthStencilView* m_DSV = nullptr;
	ID3D11Buffer* m_VSConstBuffer = nullptr;

	class HWD3DRenderState_DX11* m_Shader = nullptr;

	hwd3d_matrix m_Proj = HWD3DMatrix_Ident;
	hwd3d_matrix m_View = HWD3DMatrix_Ident;
	hwd3d_matrix m_World = HWD3DMatrix_Ident;
	hwd3d_matrix m_ShaderWVP = HWD3DMatrix_Ident;

public:
	
	ID3D11Device* GetDevice() const { return m_D3DDevice; }
	ID3D11DeviceContext* GetContext() const { return m_D3DContext; }

private:

	virtual void InitDevice( HWND TargetWnd) override;
	virtual void DeinitDevice() override;

	virtual void ClearViewport() override;
	virtual bool BeginDraw() override;
	virtual void EndDraw() override;
	virtual void Present() override;

	virtual void SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix) override;

	bool InitSharedObjects();
};
