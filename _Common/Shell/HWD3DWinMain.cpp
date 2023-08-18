// D3D Hello World

#include "HWD3DGame.h"

static const WCHAR WinMain_Title[] = L"Hello World D3D";
static const WCHAR WinMain_WindowClass[] = L"HelloWorldD3D";

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if (_WIN32_WINNT >= 0x0600)
	{
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
	}
#endif

	const POINT DisplayRes = { 800 , 600 }; // TODO: Command Line Parameter
	const float GameUpdateRate = 1.f/60.f;
	const int MaxUpatesPerFrameThreshold = 10;

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
		MainWnd = CreateWindowW(WinMain_WindowClass, WinMain_Title, WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

		if (!MainWnd)
		{
			return -1;
		}

		// Adjust window to be our resolution size:
		RECT RcSize = { 0 , 0 , DisplayRes.x , DisplayRes.y };
		const LONG Style = GetWindowLongW( MainWnd , GWL_STYLE );
		const LONG ExStyle = GetWindowLongW( MainWnd , GWL_EXSTYLE );
		AdjustWindowRectEx( &RcSize , Style , TRUE , ExStyle );
		SetWindowPos( MainWnd , NULL , 0 , 0 , RcSize.right - RcSize.left , RcSize.bottom - RcSize.top , SWP_NOREPOSITION|SWP_NOMOVE );
		GetClientRect( MainWnd , &MainWnd_ClientRect );

		ShowWindow(MainWnd, nCmdShow);
		UpdateWindow(MainWnd);
	}

	const HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D3D3));

	HWD3DGame* Game = HWD3DGame::CreateGame(MainWnd);
	bool bGameStillRunning = true;

	LARGE_INTEGER PerfFreq = { };
	QueryPerformanceFrequency( &PerfFreq );
	auto GetRawTimeElapsedSec =[&PerfFreq]( const LARGE_INTEGER& Start , const LARGE_INTEGER& End ) -> float
	{
		assert( Start.QuadPart <= End.QuadPart );
		const LONGLONG Diff = End.QuadPart - Start.QuadPart;
		const double TimeMs = (Diff * 1000.) / PerfFreq.QuadPart;
		const float TimeSec = static_cast<float>(TimeMs) / 1000.f;
		return TimeSec;
	};

	LARGE_INTEGER FrameStartTime = { };
	LARGE_INTEGER FrameEndTime = { };
	QueryPerformanceCounter( &FrameStartTime );
	FrameEndTime = FrameStartTime;
	float CumulativeDelta = 0.f;

	while (bGameStillRunning)
	{
		const float GameDeltaTime = GetRawTimeElapsedSec( FrameStartTime , FrameEndTime );
		const float TotalDelta = GameDeltaTime + CumulativeDelta;
		CumulativeDelta = TotalDelta;

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
			int NumUpdatesThisFrame = 0;
			while( CumulativeDelta >= GameUpdateRate )
			{
				Game->Update(GameUpdateRate);
				CumulativeDelta -= GameUpdateRate;
				NumUpdatesThisFrame++;

				if( NumUpdatesThisFrame >= MaxUpatesPerFrameThreshold )
				{
					CumulativeDelta = 0.f;
					break;
				}
			}
			Game->DrawScene();
		}

		FrameStartTime = FrameEndTime;
		QueryPerformanceCounter( &FrameEndTime );
	}

	HWD3D_SafeRelease(Game);

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
