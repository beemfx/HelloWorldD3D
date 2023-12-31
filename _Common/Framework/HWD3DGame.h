// D3D Hello World

#pragma once

#include "HWD3DObject.h"
#include "HWD3DTypes.h"

class HWD3DGame : public HWD3DObject
{
public:
	
	static HWD3DGame* CreateGame(HWND InMainWnd);

protected:

	enum class hwd3d_transform_t
	{
		Proj ,
		View ,
		World ,
	};

private:
	
	hwd3d_vec2 m_ViewSize = hwd3d_vec2(1.f, 1.f);
	int m_Frame = 0;

	struct hwd3dMeshSet
	{
		class HWD3DMesh* Mesh = nullptr;
		class HWD3DTexture* Texture = nullptr;
		hwd3d_matrix WorldMatrix = HWD3DMatrix_Ident;
		hwd3d_vec3 UserTranslation = HWD3DVec3_Zero;

		void Load(HWD3DGame& InGame, const char* InMeshFile, const char* InTextureFile);
		void Unload();
		void Draw(HWD3DGame& InGame);
	};

	hwd3dMeshSet m_Teapot;
	hwd3dMeshSet m_Cube;

	float m_MeshRotationTime = 0.f;
	const float m_MeshRotationDuration = 5.f;

public:
	
	void Update(float DeltaTime);
	void DrawScene();

protected:
	

	virtual void OnObjectDestruct() override;

	void CreateScene();
	void DestroyScene();

	virtual void InitDevice(HWND InMainWnd) { }
	virtual void DeinitDevice() { }

	virtual void ClearViewport() { }
	virtual bool BeginDraw() { return true; }
	virtual void EndDraw() { }
	virtual void Present() { }

	virtual void SetTransformMatrix(hwd3d_transform_t InType, const hwd3d_matrix& InMatrix) { }

protected:

	void Init(HWND InMainWnd);
	void Deinit();
};

