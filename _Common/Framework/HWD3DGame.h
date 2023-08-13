#pragma once

#include "HWD3DTypes.h"

class HWD3DGame
{
public:
	
	static HWD3DGame* CreateGame(HWND InMainWnd);

public:
	
	virtual void Update(float DeltaTime);
	virtual void Render();

	void Release();

protected:

	virtual void Init(HWND InMainWnd);
	virtual void Deinit();
};

