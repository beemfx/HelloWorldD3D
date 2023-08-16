// D3D Hello World

#pragma once

#include "HWD3DTypes.h"

class HWD3DMesh
{
public:

	static HWD3DMesh* CreateMesh(class HWD3DGame* InGame, const char* InFilename); // Must be overridden per project.

protected:
	
	std::vector<hwd3d_vertex> m_Vertices;
	std::vector<hwd3d_triangle> m_Triangles;

public:

	void Release();

	virtual void Draw() { }

protected:

	virtual ~HWD3DMesh() { }

protected:

	void LoadMeshFile(const char* InFilename);
	void ClearMeshFile();
};

