// D3D Hello World

#pragma once

#include "HWD3DTypes.h"

class HWD3DTexture
{
public:
	
	static HWD3DTexture* CreateTexture(class HWD3DGame* InGame, const char* InFilename); // Must be overridden per project.

protected:
	
	int m_Width = 0;
	int m_Height = 0;
	std::vector<hwd3d_rgba> m_Pixels;

public:
	
	void Release();

protected:
	
	void LoadTextureFile(const char* InFilename);
	void ClearImage();
};

