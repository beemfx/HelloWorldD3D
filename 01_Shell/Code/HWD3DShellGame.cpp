// D3D Hello World

#include "HWD3DShellGame.h"
#include "HWD3DMesh.h"
#include "HWD3DTexture.h"

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	HWD3DGame* Out = new HWD3DGame_Shell;
	if( Out )
	{
		Out->Init(InMainWnd);
	}

	return Out;
}

HWD3DMesh* HWD3DMesh::CreateMesh(class HWD3DGame* InGame, const char* InFilename)
{
	return nullptr;
}

HWD3DTexture* HWD3DTexture::CreateTexture(class HWD3DGame* InGame, const char* InFilename)
{
	return nullptr;
}
