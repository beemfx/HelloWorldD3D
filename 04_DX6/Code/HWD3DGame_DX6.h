// D3D Hello World

#include "HWD3DGame.h"

#include <d3d.h>
#include <ddraw.h>

class HWD3DGame_DX6 : public HWD3DGame
{
private:
	
	HWND m_TargetWnd = nullptr;
	IDirectDraw4* m_DDraw = nullptr;
	IDirect3D3* m_D3D = nullptr;
	IDirectDrawSurface4* m_PrimarySurface = nullptr;
	IDirectDrawSurface4* m_BackBuffer = nullptr;
	IDirectDrawSurface4* m_ZBuffer = nullptr;

	IDirect3DDevice3* m_D3DDevice = nullptr;
	IDirect3DViewport3* m_Viewport = nullptr;

	DWORD m_MemFlag = 0;

	struct d3dDeviceData
	{
		std::string Name;
		GUID Id = { };
		bool bUseVideoMem = false;
	};

	std::vector<d3dDeviceData> m_DevicesFound;

public:
	
	IDirectDraw4* GetDirectDraw() const { return m_DDraw; }
	IDirect3D3* GetD3D() const { return m_D3D; }
	IDirect3DDevice3* GetDevice() const { return m_D3DDevice; }
	DWORD GetMemFlag() const { return m_MemFlag; }

private:

	virtual void InitDevice( HWND TargetWnd) override;
	virtual void DeinitDevice() override;

	virtual void ClearViewport() override;
	virtual bool BeginDraw() override;
	virtual void EndDraw() override;
	virtual void Present() override;

	virtual void SetWorldMatrix(const hwd3d_matrix& InMatrix) override;

	void InitCommonStates();

	static HRESULT FAR PASCAL D3DCb_EnumDevices(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC DevDesc1, LPD3DDEVICEDESC DevDesc2, LPVOID Context);
	static HRESULT WINAPI D3DCb_EnumZBufferFormat( DDPIXELFORMAT* pddpf, VOID* pddpfDesired );
};
