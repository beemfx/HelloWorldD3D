//

#include "HWD3DShellGame.h"

HWD3DGame* HWD3DGame::CreateGame(HWND InMainWnd)
{
	HWD3DGame* Out = new HWD3DGame_Shell;
	if( Out )
	{
		Out->Init(InMainWnd);
	}

	return Out;
}


void HWD3DGame_Shell::Init(HWND InMainWnd)
{
	
}

void HWD3DGame_Shell::Deinit()
{
	
}

void HWD3DGame_Shell::Update(float DeltaTime)
{
	
}

void HWD3DGame_Shell::Render()
{
	
}
