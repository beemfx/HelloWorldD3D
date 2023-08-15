// D3D Hello World

#include "HWD3DTexture.h"

void HWD3DTexture::Release()
{
	delete this;
}

void HWD3DTexture::LoadTextureFile(const char* InFilename)
{
	ClearImage();

	// We're making assumptions about endianness here, but for simple
	// hello world that is only running on x86 we consider that acceptable.
	const int HEADER_VALUE = 'A' << 24 | 'B' << 16 | 'G' << 8 | 'R' << 0;

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
		ClearImage();
		return;
	}

	const BOOL bReadWidth = ReadFile(hFile, &m_Width, sizeof(m_Width), &NumRead, NULL);
	if (!bReadWidth || NumRead != sizeof(m_Width))
	{
		CloseHandle(hFile);
		ClearImage();
		return;
	}

	const BOOL bReadHeight = ReadFile(hFile, &m_Height, sizeof(m_Height), &NumRead, NULL);
	if (!bReadHeight || NumRead != sizeof(m_Height))
	{
		CloseHandle(hFile);
		ClearImage();
		return;
	}

	// Sanity check
	if (m_Width > 4096 || m_Height > 4096)
	{
		CloseHandle(hFile);
		ClearImage();
		return;
	}

	m_Pixels.resize(m_Width * m_Height);

	const DWORD PixelsSize = m_Width * m_Height * sizeof(hwd3d_rgba);
	const BOOL bReadPixels = ReadFile(hFile, m_Pixels.data(), PixelsSize, &NumRead, NULL);

	if (!bReadPixels || PixelsSize != NumRead)
	{
		CloseHandle(hFile);
		ClearImage();
		return;
	}

	CloseHandle(hFile);
}

void HWD3DTexture::ClearImage()
{
	m_Width = 0;
	m_Height = 0;
	m_Pixels.resize(0);
}
