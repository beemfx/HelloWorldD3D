// D3D Hello World

#include "HWD3DGame.h"
#include <d3d10.h>

class HWD3DGame_DX10 : public HWD3DGame
{
private:
	
	HWND m_TargetWnd = nullptr;
	UINT m_ViewWidth = 0;
	UINT m_ViewHeight = 0;

	IDXGISwapChain* m_SwapChain = nullptr;
	ID3D10Device* m_D3DDevice = nullptr;
	ID3D10Texture2D* m_RTVTexture = nullptr;
	ID3D10RenderTargetView* m_RTV = nullptr;
	ID3D10Texture2D* m_DSVTexture = nullptr;
	ID3D10DepthStencilView* m_DSV = nullptr;
	ID3D10Buffer* m_VSConstBuffer = nullptr;
	ID3D10RasterizerState* m_RS = nullptr;
	ID3D10SamplerState* m_SS = nullptr;

	class HWD3DShader_DX10* m_Shader = nullptr;

	hwd3d_matrix m_Proj = HWD3DMatrix_Ident;
	hwd3d_matrix m_View = HWD3DMatrix_Ident;
	hwd3d_matrix m_World = HWD3DMatrix_Ident;
	hwd3d_matrix m_ShaderWVP = HWD3DMatrix_Ident;

public:
	
	// IDirect3D9* GetD3D() const { return m_D3D; }
	ID3D10Device* GetDevice() const { return m_D3DDevice; }

private:

	virtual void InitDevice( HWND TargetWnd) override;
	virtual void DeinitDevice() override;

	virtual void ClearViewport() override;
	virtual bool BeginDraw() override;
	virtual void EndDraw() override;
	virtual void Present() override;

	virtual void SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix) override;

	bool CreateConstBuffer();
};
