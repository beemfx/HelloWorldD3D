// D3D Hello World

#include "HWD3DSysInclude_VS6.h"
// #include "HWD3DGame.h"
// #include "HWD3DTimer.h"

#pragma comment(lib, "Winmm.lib")

#define IDM_EXIT 1

static const char WinMain_Title[] = "Hello World D3D";
static const char WinMain_WindowClass[] = "HelloWorldD3D";

static LRESULT CALLBACK HWD3DWinMain_WndProc(HWND, UINT, WPARAM, LPARAM);
static BOOL HWD3DWinMain_ChangeToContentDirectory();
static void HWD3DWinMain_SetDPIAware();

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HWD3DWinMain_SetDPIAware();
	HWD3DWinMain_ChangeToContentDirectory();

	const POINT DisplayRes = { 800, 600 }; // TODO: Command Line Parameter
	const float GameUpdateRate = 1.f / 60.f;
	const int MaxUpatesPerFrameThreshold = 10;
	const DWORD UpdateRateMs = (DWORD)(floorf(1000 * GameUpdateRate));

	HWND MainWnd = NULL;

	// Register Window Class:
	{
		WNDCLASSEXA wcex;
		ZeroMemory(&wcex, sizeof(wcex));
		wcex.cbSize = sizeof(wcex);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = HWD3DWinMain_WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIconA(hInstance, "IDI_APPICON");
		wcex.hCursor = LoadCursorA(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = "IDC_APPMENU";
		wcex.lpszClassName = WinMain_WindowClass;
		wcex.hIconSm = LoadIconA(wcex.hInstance, "IDI_APPICON");

		ATOM RegisterClassAtom = RegisterClassExA(&wcex);
		if (!RegisterClassAtom)
		{
			return -1;
		}
	}

	// Create Main Window
	{
		MainWnd = CreateWindowA(WinMain_WindowClass, WinMain_Title, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

		if (!MainWnd)
		{
			return -1;
		}
	}

	// Adjust window to be our resolution size:
	{
		RECT RcSize = { 0, 0, DisplayRes.x, DisplayRes.y };
		const LONG Style = GetWindowLongW(MainWnd, GWL_STYLE);
		const LONG ExStyle = GetWindowLongW(MainWnd, GWL_EXSTYLE);
		AdjustWindowRectEx(&RcSize, Style, TRUE, ExStyle);
		SetWindowPos(MainWnd, NULL, 0, 0, RcSize.right - RcSize.left, RcSize.bottom - RcSize.top, SWP_NOREPOSITION | SWP_NOMOVE);

		ShowWindow(MainWnd, nCmdShow);
		UpdateWindow(MainWnd);
	}

	// HWD3DGame* Game = HWD3DGame::CreateGame(MainWnd);
	// if (!Game)
	// {
	// 	DestroyWindow(MainWnd);
	// 	return -1;
	// }

	BOOL bGameStillRunning = TRUE;

	static const DWORD MaxUpdatesThreshold = 10;
	DWORD LastUpdateTime = timeGetTime();

	while (bGameStillRunning)
	{
		// Pump Messages
		MSG msg;
		while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				bGameStillRunning = FALSE;
			}
		}

		if (bGameStillRunning)
		{
			const DWORD FrameStartTime = timeGetTime();
			const DWORD TimeSinceLastUpdate = FrameStartTime - LastUpdateTime;
			DWORD NumUpdatesThisFrame = 0;
			while( TimeSinceLastUpdate >= UpdateRateMs )
			{
				// TODO: Game Update (GameUpdateRate)

				LastUpdateTime += UpdateRateMs;
				NumUpdatesThisFrame++;
				if (NumUpdatesThisFrame >= MaxUpdatesThreshold)
				{
					// Too man updates this frame, catch up.
					LastUpdateTime = FrameStartTime;
					break;
				}
			}
			
			// TODO: Draw Scene
		}
	}

	// HWD3D_SafeRelease(Game);

	return 0;
}

static LRESULT CALLBACK HWD3DWinMain_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		const int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProcA(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcA(hWnd, message, wParam, lParam);
	}
	return 0;
}

static BOOL HWD3DWinMain_ChangeToContentDirectory()
{
	static const int MAX_PARENT_DIRS = 10;

	for (int i = 0; i < MAX_PARENT_DIRS; i++)
	{
		// Is the _Media directory in our working directory, if so we're where we want to be:
		const DWORD Attrs = GetFileAttributesA("_Media");
	#ifndef INVALID_FILE_ATTRIBUTES
		#define INVALID_FILE_ATTRIBUTES -1 // Our DX8 version of the Windows API doesn't have this defined.
	#endif
		if (INVALID_FILE_ATTRIBUTES != Attrs && ((Attrs&FILE_ATTRIBUTE_DIRECTORY) != 0))
		{
			return TRUE;
		}

		// If not go up one directory:
		if (!SetCurrentDirectoryA(".."))
		{
			return FALSE;
		}
	}

	return FALSE;
}

static void HWD3DWinMain_SetDPIAware()
{
#define HWD3DWINMAIN_CALL_DLL_DPI_AWARENESS 1

#if HWD3DWINMAIN_CALL_DLL_DPI_AWARENESS
	// Load and call SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE) directly from DLL.
	// Since we're getting this function from the DLL, if it doesn't exist on this version of Windows, it
	// simply won't be called.
	HMODULE WinUserLib = LoadLibraryA("User32.dll");
	if (WinUserLib)
	{
		typedef BOOL(WINAPI* FnType)(_In_ HANDLE value);
		FnType Fn = (FnType)(GetProcAddress(WinUserLib, "SetProcessDpiAwarenessContext"));
		if (Fn)
		{
			Fn(((HANDLE)(-3)));
		}
		FreeLibrary(WinUserLib);
	}
#else
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
#endif

#undef HWD3DWINMAIN_CALL_DLL_DPI_AWARENESS
}
