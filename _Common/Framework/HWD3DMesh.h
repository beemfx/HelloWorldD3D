// D3D Hello World

#pragma once

#include "HWD3DObject.h"
#include "HWD3DTypes.h"

class HWD3DMesh : public HWD3DObject
{
public:

	static HWD3DMesh* CreateMesh(class HWD3DGame* InGame, const char* InFilename); // Must be overridden per project.

protected:
	
	std::vector<hwd3d_vertex> m_Vertices;
	std::vector<hwd3d_triangle> m_Triangles;

public:
	
	virtual void Draw() { }

protected:

	void LoadMeshFile(const char* InFilename);
	void LoadSampleCube();
	void ClearMeshFile();
};

