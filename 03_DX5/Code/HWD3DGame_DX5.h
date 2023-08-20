// D3D Hello World

#include "HWD3DGame.h"

#include <d3d.h>
#include <ddraw.h>

class HWD3DGame_DX5 : public HWD3DGame
{
private:
	
	HWND m_TargetWnd = nullptr;
	IDirectDraw2* m_DDraw = nullptr;
	IDirect3D2* m_D3D = nullptr;
	IDirectDrawSurface* m_PrimarySurface = nullptr;
	IDirectDrawSurface* m_BackBuffer = nullptr;
	IDirectDrawSurface* m_ZBuffer = nullptr;

	IDirect3DDevice2* m_D3DDevice = nullptr;
	IDirect3DViewport2* m_Viewport = nullptr;

	IDirect3DMaterial2* m_BgMaterial = nullptr;
	D3DMATERIALHANDLE m_BgMaterialHandle = 0;

	DWORD m_MemFlag = 0;

	struct d3dDeviceData
	{
		std::string Name;
		GUID Id = { };
		bool bUseVideoMem = false;
	};

	std::vector<d3dDeviceData> m_DevicesFound;

public:
	
	IDirectDraw2* GetDirectDraw() const { return m_DDraw; }
	IDirect3DDevice2* GetDevice() const { return m_D3DDevice; }
	DWORD GetMemFlag() const { return m_MemFlag; }

private:

	virtual void InitDevice( HWND TargetWnd) override;
	virtual void DeinitDevice() override;

	virtual void ClearViewport() override;
	virtual bool BeginDraw() override;
	virtual void EndDraw() override;
	virtual void Present() override;

	virtual void SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix) override;

	static HRESULT FAR PASCAL D3DCb_EnumDevices(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC DevDesc1, LPD3DDEVICEDESC DevDesc2, LPVOID Context);
};
