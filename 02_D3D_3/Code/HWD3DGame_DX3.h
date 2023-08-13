//

#include "HWD3DGame.h"

#include <d3d.h>
#include <ddraw.h>
// #include <ddrawex.h>
#include <vector>
#include <string>
#include <guiddef.h>
#include <cassert>

class HWD3DGame_DX3 : public HWD3DGame
{
private:
	
	HWND m_TargetWnd = nullptr;
	IDirectDraw* m_DDraw = nullptr;
	IDirect3D* m_D3D = nullptr;
	IDirectDrawSurface* m_PrimarySurface = nullptr;
	IDirectDrawSurface* m_BackBuffer = nullptr;

	IDirect3DDevice* m_D3DDevice = nullptr;
	D3DEXECUTEBUFFERDESC m_ExecBufferDesc = { };
	IDirect3DExecuteBuffer* m_ExecBuffer = nullptr;
	IDirect3DViewport* m_Viewport = nullptr;
	IDirect3DMaterial* m_Material = nullptr;
	D3DMATERIALHANDLE m_MaterialHandle = 0;

	IDirect3DMaterial* m_BgMaterial = nullptr;
	D3DMATERIALHANDLE m_BgMaterialHandle = 0;

	D3DMATRIXHANDLE m_MatrixProj = 0;
	D3DMATRIXHANDLE m_MatrixView = 0;
	D3DMATRIXHANDLE m_MatrixWorld = 0;

	int m_Frame = 0;

	struct d3dDeviceData
	{
		std::string Name;
		GUID Id;
	};

	std::vector<d3dDeviceData> m_DevicesFound;

public:

	virtual void Init( HWND TargetWnd) override;
	virtual void Deinit() override;

	virtual void Update(float DeltaTime) override;
	virtual void Render() override;

	void CreateExecBuffer();

	static HRESULT FAR PASCAL D3DCb_EnumDevices(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC DevDesc1, LPD3DDEVICEDESC DevDesc2, LPVOID Context);
};
