// D3D Hello World

#include "HWD3DGame.h"
#include "HWD3DTimer.h"

#define IDM_EXIT 1

static const WCHAR WinMain_Title[] = L"Hello World D3D";
static const WCHAR WinMain_WindowClass[] = L"HelloWorldD3D";

namespace HWD3DWinMain
{
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static bool ChangeToContentDirectory();
	static void SetDPIAware();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	using namespace HWD3DWinMain;

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	SetDPIAware();
	ChangeToContentDirectory();

	const POINT DisplayRes = { 800, 600 }; // TODO: Command Line Parameter
	const float GameUpdateRate = 1.f / 60.f;
	const int MaxUpatesPerFrameThreshold = 10;

	HWND MainWnd = NULL;

	// Register Window Class:
	{
		WNDCLASSEXW wcex = { };
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIconW(hInstance, L"IDI_APPICON");
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = L"IDC_APPMENU";
		wcex.lpszClassName = WinMain_WindowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, L"IDI_APPICON");

		ATOM RegisterClassAtom = RegisterClassExW(&wcex);
		if (!RegisterClassAtom)
		{
			return -1;
		}
	}

	// Create Main Window
	{
		MainWnd = CreateWindowW(WinMain_WindowClass, WinMain_Title, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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

	HWD3DGame* Game = HWD3DGame::CreateGame(MainWnd);
	if (!Game)
	{
		DestroyWindow(MainWnd);
		return -1;
	}

	bool bGameStillRunning = true;

	HWD3DTimer FrameTimer;

	while (bGameStillRunning)
	{
		// Pump Messages
		MSG msg = {};
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				bGameStillRunning = false;
			}
		}

		if (bGameStillRunning)
		{
			FrameTimer.UpdateAtRate(GameUpdateRate, MaxUpatesPerFrameThreshold, [Game](float DeltaTime) -> void 
				{
					Game->Update(DeltaTime); 
				} );

			Game->DrawScene();
		}
	}

	HWD3D_SafeRelease(Game);

	return 0;
}

static LRESULT CALLBACK HWD3DWinMain::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

static bool HWD3DWinMain::ChangeToContentDirectory()
{
	static const int MAX_PARENT_DIRS = 10;

	for (int i = 0; i < MAX_PARENT_DIRS; i++)
	{
		// Is the _Media directory in our working directory, if so we're where we want to be:
		const DWORD Attrs = GetFileAttributesW(L"_Media");
	#ifndef INVALID_FILE_ATTRIBUTES
		#define INVALID_FILE_ATTRIBUTES -1 // Our DX8 version of the Windows API doesn't have this defined.
	#endif
		if (INVALID_FILE_ATTRIBUTES != Attrs && ((Attrs&FILE_ATTRIBUTE_DIRECTORY) != 0))
		{
			return true;
		}

		// If not go up one directory:
		if (!SetCurrentDirectoryW(L".."))
		{
			return false;
		}
	}

	return false;
}

static void HWD3DWinMain::SetDPIAware()
{
#define HWD3DWINMAIN_CALL_DLL_DPI_AWARENESS 1

#if HWD3DWINMAIN_CALL_DLL_DPI_AWARENESS
	// Load and call SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE) directly from DLL.
	// Since we're getting this function from the DLL, if it doesn't exist on this version of Windows, it
	// simply won't be called.
	HMODULE WinUserLib = LoadLibraryW(L"User32.dll");
	if (WinUserLib)
	{
		typedef BOOL(WINAPI* FnType)(_In_ HANDLE value);
		FnType Fn = reinterpret_cast<FnType>(GetProcAddress(WinUserLib, "SetProcessDpiAwarenessContext"));
		if (Fn)
		{
			Fn((reinterpret_cast<HANDLE>(-3)));
		}
		FreeLibrary(WinUserLib);
	}
#else
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
#endif

#undef HWD3DWINMAIN_CALL_DLL_DPI_AWARENESS
}
