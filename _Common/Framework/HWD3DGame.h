// D3D Hello World

#pragma once

#include "HWD3DTypes.h"

class HWD3DGame
{
public:
	
	static HWD3DGame* CreateGame(HWND InMainWnd);

protected:
	
	int m_Frame = 0;

	class HWD3DTexture* m_Texture = nullptr;
	class HWD3DMesh* m_Mesh = nullptr;

	hwd3d_matrix m_MeshMatrix = HWD3DMatrix_Ident;
	float m_MeshRotationTime = 0.f;
	const float m_MeshRotationDuration = 5.f;

public:
	
	virtual void Initialize() { }

	void Update(float DeltaTime);
	void DrawScene();

	virtual void ClearViewport();
	virtual bool BeginDraw();
	virtual void EndDraw();
	virtual void Present();

	virtual void SetWorldMatrix(const hwd3d_matrix& InMatrix);

	void Release();

protected:

	virtual void Init(HWND InMainWnd);
	virtual void Deinit();
};

