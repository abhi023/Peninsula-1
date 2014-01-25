#include "pch.h"
#include "peninsula.h"
#include "Graphics.h"
#include "Control.h"
#include "resource.h"
#include "matrix.h"

array<bool, 256> bKeys;

CGraphics g_Graphics;
Controls g_Controls;

#ifdef _DEBUG
typedef HRESULT(WINAPI *debugInterface)(REFIID, void**);
#endif 

bool failed(HRESULT hr)
{
	return hr < 0;
}
bool failed(bool b)
{
	return !b;
}
bool succeeded(HRESULT hr)
{
	return !failed(hr);
}
bool succeeded(bool b)
{
	return !failed(b);
}

std::string wstring_to_string(const std::wstring& wstr)
{
	return string(wstr.begin(), wstr.end());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		g_Controls.mouse_x = GET_X_LPARAM(lParam);
		g_Controls.mouse_y = GET_Y_LPARAM(lParam);
		break;

	case WM_KEYDOWN:
		bKeys[wParam] = true;
		if (bKeys[VK_ESCAPE] == true) PostQuitMessage(0);
		break;

	case WM_KEYUP:
		bKeys[wParam] = false;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, char*, int)
{
	matrix<double, 2, 2> m({ { 1, 2 }, { 3, 4 } });

	try
	{
		int argc;
		wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
		
		vector<wstring> arguments;
		for (int i = 0; i < argc; i++)
		{
			arguments.push_back(argv[i]);
		}
		
		HRESULT hr = CoInitialize(0);
		if (failed(hr))
			throw error(__FILE__, __LINE__, hr);
	
		WNDCLASSEX wc;
		memset(&wc, 0, sizeof(wc));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hInstance = hInst;
		wc.lpfnWndProc = WndProc;
		wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
		wc.lpszClassName = L"Peninsula";
		wc.style = CS_HREDRAW | CS_VREDRAW;
		if (RegisterClassEx(&wc) == FALSE)
			throw error(__FILE__, __LINE__, HRESULT_FROM_WIN32(GetLastError()));

		const DWORD dwStyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_VISIBLE;
		const DWORD dwExStyle = WS_EX_APPWINDOW;

		RECT r = {0, 0, 800, 600};
		if (AdjustWindowRectEx(&r, dwStyle, FALSE, dwExStyle) == FALSE)
			throw error(__FILE__, __LINE__, HRESULT_FROM_WIN32(GetLastError()));

		HWND hWnd = CreateWindowEx(dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, nullptr, nullptr, hInst, nullptr);

		if (IsWindow(hWnd) == FALSE)
		{
			throw error(__FILE__, __LINE__, HRESULT_FROM_WIN32(GetLastError()));
		}

		g_Graphics.Initialise(hWnd);

		SetTimer(hWnd, TIMER_EVENT_GAMELOGIC, 1000 / 60, &Controls::ControlTimerProc);

		MSG msg;
		ZeroMemory(&msg);

		do 
		{
			if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
			else
			{
				g_Graphics.Render();
			}
		}
		while (msg.message != WM_QUIT);

		return static_cast<int>(msg.message);
	}
	catch (const error& excp)
	{
		std::string str = "An exception has occured.\nFile: " + excp.file + "\nLine: " + to_string(excp.line) + "\nError Code: ";
		str += wstring_to_string(_com_error(excp.ec).ErrorMessage());
		str += " (" + to_string(excp.ec) + ")";

		MessageBoxA(nullptr, str.c_str(), "Error!", MB_ICONERROR);

		return -1;
	}
}