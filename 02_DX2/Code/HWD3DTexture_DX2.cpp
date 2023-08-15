// D3D Hello World

#include "HWD3DTexture_DX2.h"

HWD3DTexture* HWD3DTexture::CreateTexture(class HWD3DGame* InGame, const char* InFilename)
{
	HWD3DTexture_DX2* Out = new HWD3DTexture_DX2;
	if (Out)
	{
		Out->LoadTextureFile(InFilename);
	}

	return Out;
}
