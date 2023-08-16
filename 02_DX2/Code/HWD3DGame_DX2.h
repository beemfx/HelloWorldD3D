// D3D Hello World

#include "HWD3DGame.h"

#include <d3d.h>
#include <ddraw.h>

class HWD3DGame_DX2 : public HWD3DGame
{
private:
	
	HWND m_TargetWnd = nullptr;
	IDirectDraw* m_DDraw = nullptr;
	IDirect3D* m_D3D = nullptr;
	IDirectDrawSurface* m_PrimarySurface = nullptr;
	IDirectDrawSurface* m_BackBuffer = nullptr;
	IDirectDrawSurface* m_ZBuffer = nullptr;

	IDirect3DDevice* m_D3DDevice = nullptr;
	IDirect3DViewport* m_Viewport = nullptr;

	IDirect3DMaterial* m_BgMaterial = nullptr;
	D3DMATERIALHANDLE m_BgMaterialHandle = 0;

	D3DMATRIXHANDLE m_MatrixProj = 0;
	D3DMATRIXHANDLE m_MatrixView = 0;
	D3DMATRIXHANDLE m_MatrixWorld = 0;
	D3DTEXTUREHANDLE m_Texture0 = 0;

	int m_Frame = 0;

	struct d3dDeviceData
	{
		std::string Name;
		GUID Id;
	};

	std::vector<d3dDeviceData> m_DevicesFound;

	class HWD3DTexture* m_Texture = nullptr;
	class HWD3DMesh* m_Mesh = nullptr;

	hwd3d_matrix m_MeshMatrix = HWD3DMatrix_Ident;
	float m_MeshRotationTime = 0.f;
	const float m_MeshRotationDuration = 5.f;

public:

	virtual void Init( HWND TargetWnd) override;
	virtual void Deinit() override;

	virtual void Update(float DeltaTime) override;
	virtual void Render() override;

	IDirectDraw* GetDirectDraw() const { return m_DDraw; }
	IDirect3DDevice* GetDevice() const { return m_D3DDevice; }
	IDirect3DViewport* GetViewport() const { return m_Viewport; }
	const D3DMATRIXHANDLE& GetProjMatrixHandle() const { return m_MatrixProj; }
	const D3DMATRIXHANDLE& GetViewMatrixHandle() const { return m_MatrixView; }
	const D3DMATRIXHANDLE& GetWorldMatrixHandle() const { return m_MatrixWorld; }
	const D3DTEXTUREHANDLE& GetTextureHandle() const { return m_Texture0; }

	void InitCommonStates();

	static HRESULT FAR PASCAL D3DCb_EnumDevices(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC DevDesc1, LPD3DDEVICEDESC DevDesc2, LPVOID Context);
};
