// D3D Hello World

#include "HWD3DMesh.h"

void HWD3DMesh::Release()
{
	delete this;
}

void HWD3DMesh::LoadMeshFile(const char* InFilename)
{
	ClearMeshFile();

	// We're making assumptions about endianness here, but for simple
	// hello world that is only running on x86 we consider that acceptable.
	const int HEADER_VALUE = 'D' << 24 | '3' << 16 | 'W' << 8 | 'H' << 0;

	HANDLE hFile = CreateFileA(InFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	int ReadHeader = 0;
	DWORD NumRead = 0;
	const BOOL bReadHeader = ReadFile(hFile, &ReadHeader, sizeof(ReadHeader), &NumRead, NULL);
	if (!bReadHeader || NumRead != sizeof(ReadHeader) || ReadHeader != HEADER_VALUE)
	{
		CloseHandle(hFile);
		ClearMeshFile();
		return;
	}

	int NumVertices = 0;
	int NumTriangles = 0;

	const BOOL bReadNumVerts = ReadFile(hFile, &NumVertices, sizeof(NumVertices), &NumRead, NULL);
	if (!bReadNumVerts || NumRead != sizeof(NumVertices))
	{
		CloseHandle(hFile);
		ClearMeshFile();
		return;
	}

	const BOOL bReadNumTris = ReadFile(hFile, &NumTriangles, sizeof(NumTriangles), &NumRead, NULL);
	if (!bReadNumTris || NumRead != sizeof(NumTriangles))
	{
		CloseHandle(hFile);
		ClearMeshFile();
		return;
	}

	// Sanity check
	if (NumVertices > 1024*1024 || NumTriangles > 1024*1024)
	{
		CloseHandle(hFile);
		ClearMeshFile();
		return;
	}

	m_Vertices.resize(NumVertices);
	m_Triangles.resize(NumTriangles);

	{
		const DWORD VertSize = NumVertices * sizeof(hwd3d_vertex);
		const BOOL bReadVerts = ReadFile(hFile, m_Vertices.data(), VertSize, &NumRead, NULL);

		if (!bReadVerts || VertSize != NumRead)
		{
			CloseHandle(hFile);
			ClearMeshFile();
			return;
		}
	}

	{
		const DWORD TriSize = NumTriangles * sizeof(hwd3d_triangle);
		const BOOL bReadTris = ReadFile(hFile, m_Triangles.data(), TriSize, &NumRead, NULL);

		if (!bReadTris || TriSize != NumRead)
		{
			CloseHandle(hFile);
			ClearMeshFile();
			return;
		}
	}

	CloseHandle(hFile);
}

void HWD3DMesh::LoadSampleCube()
{
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
