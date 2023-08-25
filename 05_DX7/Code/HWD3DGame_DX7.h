// D3D Hello World

#pragma once

#include "HWD3DGame.h"
#include <d3d.h>
#include <ddraw.h>

class HWD3DGame_DX7 : public HWD3DGame
{
private:
	
	HWND m_TargetWnd = nullptr;
	IDirectDraw7* m_DDraw = nullptr;
	IDirect3D7* m_D3D = nullptr;
	IDirectDrawSurface7* m_PrimarySurface = nullptr;
	IDirectDrawSurface7* m_BackBuffer = nullptr;
	IDirectDrawSurface7* m_ZBuffer = nullptr;

	IDirect3DDevice7* m_D3DDevice = nullptr;

	DWORD m_MemFlag = 0;

	struct d3dDeviceData
	{
		D3DDEVICEDESC7 Desc = { };
	};

	std::vector<d3dDeviceData> m_DevicesFound;

public:
	
	IDirectDraw7* GetDirectDraw() const { return m_DDraw; }
	IDirect3D7* GetD3D() const { return m_D3D; }
	IDirect3DDevice7* GetDevice() const { return m_D3DDevice; }
	DWORD GetMemFlag() const { return m_MemFlag; }

private:

	virtual void InitDevice( HWND TargetWnd) override;
	virtual void DeinitDevice() override;

	virtual void ClearViewport() override;
	virtual bool BeginDraw() override;
	virtual void EndDraw() override;
	virtual void Present() override;

	virtual void SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix) override;

	static HRESULT CALLBACK D3DCb_EnumDevices(LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpD3DDeviceDesc, LPVOID lpContext);
	static HRESULT WINAPI D3DCb_EnumZBufferFormat( DDPIXELFORMAT* pddpf, VOID* pddpfDesired );
};
