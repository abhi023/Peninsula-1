#include "pch.h"

#include "Graphics.h"
#include "Direct3D11.h"

shared_ptr<IGraphicsLayer> g_Graphics;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	try
	{
		HDC hDC;
		PAINTSTRUCT ps;

		switch (uMsg)
		{
		case WM_SIZE:
			if (g_Graphics != nullptr)
				g_Graphics->Resize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

			break;

		case WM_PAINT:
			hDC = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	catch (...)
	{
		ExceptionHandler();
		return ERROR;
	}
}

LRESULT CALLBACK TreeDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		break;

	case WM_SIZE:
		break;

	case WM_COMMAND:
		break;

	case WM_DESTROY:
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, char*, int)
{
	try 
	{
		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.hInstance = hInst;
		wc.lpfnWndProc = WndProc;
		wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
		wc.lpszClassName = L"WorldBuilder";
		wc.style = CS_HREDRAW | CS_VREDRAW;
		if (!RegisterClassEx(&wc))
			throw "Failed to register window class";

		const DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE,
			dwExStyle = WS_EX_APPWINDOW;

		HWND hWnd = CreateWindowEx(dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, nullptr, nullptr, hInst, nullptr);
		if (!hWnd)
			throw "Failed to create main window";

		wc.lpfnWndProc = TreeDlgProc;
		wc.lpszClassName = L"TreeDlg";
		if (!RegisterClassEx(&wc))
			throw "Failed to register window class - tree dialog";

		HWND hDlgWnd = CreateWindowEx(0, wc.lpszClassName, L"Scene", WS_DLGFRAME | WS_VISIBLE | WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 200, CW_USEDEFAULT, hWnd, nullptr, hInst, nullptr);
		if (!hDlgWnd)
			throw Win32Exception();

		g_Graphics = make_shared<Direct3D11>();
		g_Graphics->Initialise(hWnd);

		MSG msg;
		do
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				g_Graphics->Render();
			}
		} while (msg.message != WM_QUIT);

		return static_cast<int>(msg.wParam);
	}
	catch (...)
	{
		ExceptionHandler();
		return -1;
	}
}