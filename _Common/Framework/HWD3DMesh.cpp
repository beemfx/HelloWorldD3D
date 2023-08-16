// D3D Hello World

#include "HWD3DMesh.h"

void HWD3DMesh::Release()
{
	delete this;
}

void HWD3DMesh::LoadMeshFile(const char* InFilename)
{
	ClearMeshFile();

	/* Cube vertices, normals, shades, and modeling transform */
	static int NumVertices = 24;
	static hwd3d_vertex CubeVertices[] =
	{
		{ { (-1.f), (1.f), (1.f)  } , { (0.f), (1.f), (0.f) }, { (1.f), (0.f) } },
		{ { (1.f) , (1.f), (1.f)  } , { (0.f), (1.f), (0.f) }, { (1.f), (1.f) } },
		{ { (1.f) , (1.f), (-1.f) } , { (0.f), (1.f), (0.f) }, { (0.f), (1.f) } },
		{ { (-1.f), (1.f), (-1.f) } , { (0.f), (1.f), (0.f) }, { (0.f), (0.f) } },

		{ { (1.f) , (1.f) , (-1.f) } , { (0.f), (0.f), (-1.f) } , { (1.f), (0.f) } },
		{ { (1.f) , (-1.f), (-1.f) } , { (0.f), (0.f), (-1.f) } , { (1.f), (1.f) } },
		{ { (-1.f), (-1.f), (-1.f) } , { (0.f), (0.f), (-1.f) } , { (0.f), (1.f) } },
		{ { (-1.f), (1.f) , (-1.f) } , { (0.f), (0.f), (-1.f) } , { (0.f), (0.f) } },
		
		{ { (1.f), (1.f) , (1.f) , } , { (1.f), (0.f), (0.f)  } , { (1.f), (0.f) } },
		{ { (1.f), (-1.f), (1.f) , } , { (1.f), (0.f), (0.f)  } , { (1.f), (1.f) } },
		{ { (1.f), (-1.f), (-1.f), } , { (1.f), (0.f), (0.f)  } , { (0.f), (1.f) } },
		{ { (1.f), (1.f) , (-1.f), } , { (1.f), (0.f), (0.f)  } , { (0.f), (0.f) } },
		
		{ { (-1.f), (1.f) , (1.f), } , { (0.f), (0.f), (1.f)  } , { (1.f), (0.f) } },
		{ { (-1.f), (-1.f), (1.f), } , { (0.f), (0.f), (1.f)  } , { (1.f), (1.f) } },
		{ { (1.f) , (-1.f), (1.f), } , { (0.f), (0.f), (1.f)  } , { (0.f), (1.f) } },
		{ { (1.f) , (1.f) , (1.f), } , { (0.f), (0.f), (1.f)  } , { (0.f), (0.f) } },
		
		{ { (-1.f), (-1.f), (-1.f) } , { (-1.f), (0.f), (0.f) } , { (0.f), (1.f) } },
		{ { (-1.f), (-1.f), (1.f)  } , { (-1.f), (0.f), (0.f) } , { (1.f), (1.f) } },
		{ { (-1.f), (1.f) , (1.f)  } , { (-1.f), (0.f), (0.f) } , { (1.f), (0.f) } },
		{ { (-1.f), (1.f) , (-1.f) } , { (-1.f), (0.f), (0.f) } , { (0.f), (0.f) } },
		
		{ { (1.f) , (-1.f), (-1.f) } , { (0.f), (-1.f), (0.f) } , { (1.f), (1.f) } },
		{ { (1.f) , (-1.f), (1.f)  } , { (0.f), (-1.f), (0.f) } , { (0.f), (1.f) } },
		{ { (-1.f), (-1.f), (1.f)  } , { (0.f), (-1.f), (0.f) } , { (0.f), (0.f) } },
		{ { (-1.f), (-1.f), (-1.f) } , { (0.f), (-1.f), (0.f) } , { (1.f), (0.f) } },
	};

	//*** Cube edges - ordered indices into the vertex array
	// static const int NumTri = 12;
	static const hwd3d_triangle CubeTri[] =
	{
		{ 0 , 1 , 2  } , { 0 , 2 , 3  },
		{ 4 , 5 , 6  } , { 4 , 6 , 7  },
		{ 8 , 9 , 10 } , { 8 , 10, 11 },
		{ 12, 13, 14 } , { 12, 14, 15 },
		{ 16, 17, 18 } , { 16, 18, 19 },
		{ 20, 21, 22 } , { 20, 22, 23 },
	};

	for (const auto& v : CubeVertices)
	{
		m_Vertices.push_back(v);
	}

	for (const auto& t : CubeTri)
	{
		m_Triangles.push_back(t);
	}
}

void HWD3DMesh::ClearMeshFile()
{
	m_Vertices.resize(0);
	m_Triangles.resize(0);
}
