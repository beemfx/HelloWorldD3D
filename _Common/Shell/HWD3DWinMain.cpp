// D3D_3.cpp : Defines the entry point for the application.
//

#include "HWD3DGame.h"

static const WCHAR WinMain_Title[] = L"Hello World D3D";
static const WCHAR WinMain_WindowClass[] = L"HelloWorldD3D";
static HINSTANCE WinMain_hInst = 0;

// Forward declarations of functions included in this code module:
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	const POINT DisplayRes = { 800 , 600 }; // TODO: Command Line Parameter

	HWND MainWnd = NULL;
	RECT MainWnd_ClientRect = { };

	// Register Window Class:
	{
		WNDCLASSEXW wcex = { };
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW|CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3D3));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_D3D3);
		wcex.lpszClassName = WinMain_WindowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		ATOM RegisterClassAtom = RegisterClassExW(&wcex);
		if (!RegisterClassAtom)
		{
			return -1;
		}
	}

	// Create Main Window
	{
		WinMain_hInst = hInstance; // Store instance handle in our global variable

		MainWnd = CreateWindowW(WinMain_WindowClass, WinMain_Title, WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

		if (!MainWnd)
		{
			return -1;
		}


		ShowWindow(MainWnd, nCmdShow);
		UpdateWindow(MainWnd);

		// Adjust window to be our resolution size:
		RECT RcSize = { 0 , 0 , DisplayRes.x , DisplayRes.y };
		const LONG Style = GetWindowLongW( MainWnd , GWL_STYLE );
		const LONG ExStyle = GetWindowLongW( MainWnd , GWL_EXSTYLE );
		AdjustWindowRectEx( &RcSize , Style , TRUE , ExStyle );
		SetWindowPos( MainWnd , NULL , 0 , 0 , RcSize.right - RcSize.left , RcSize.bottom - RcSize.top , SWP_NOREPOSITION|SWP_NOMOVE );
		GetClientRect( MainWnd , &MainWnd_ClientRect );
	}

	const HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D3D3));

	HWD3DGame* Game = HWD3DGame::CreateGame(MainWnd);
	bool bGameStillRunning = true;
	while (bGameStillRunning)
	{
		// Pump Messages
		MSG msg = { };
		while (PeekMessageW(&msg, nullptr, 0 , 0, PM_NOREMOVE))
		{
			GetMessage(&msg, nullptr, 0, 0);
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
				{
					bGameStillRunning = false;
				}
			}
		}

		if (bGameStillRunning)
		{
			Game->Update(1.f/60.f);
			Game->Render();
		}
	}

	Game->Release();

	return 0;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(WinMain_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
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

static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
