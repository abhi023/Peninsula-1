#include "pch.h"
#include "peninsula.h"
#include "Graphics.h"
#include "Control.h"
#include "resource.h"
#include "matrix.h"
#include "thread_messages.h"
#include "GameEntity.h"

CGraphics g_Graphics;

shared_ptr<Player> g_pHumanPlayer;

thread control_thread;

atomic<bool> quitting = false;

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

std::string to_string(const std::wstring& wstr)
{
	return string(wstr.begin(), wstr.end());
}

bool check_sse()
{
	int features[4];
	int infotype = 0;

	__cpuid(features, infotype);

	return (features[3] & 0x04000000) != 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		//if (g_Controls != nullptr)
		//{
		//	g_Controls->mouse_x = GET_X_LPARAM(lParam);
		//	g_Controls->mouse_y = GET_Y_LPARAM(lParam);
		//}
		break;

	case WM_KEYDOWN:
		break;

	case WM_KEYUP:
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

		DWORD dwStyle = WS_VISIBLE;
		const DWORD dwExStyle = WS_EX_APPWINDOW;

		int iFullScreen = MessageBox(nullptr, L"Would you like to start in Fullscreen mode?", L"Peninsula", MB_YESNOCANCEL | MB_ICONQUESTION);
		bool bFullscreen = false;

		switch (iFullScreen)
		{
		case IDYES:
			dwStyle |= WS_POPUP;
			bFullscreen = true;
			break;

		case IDNO:
			dwStyle |= WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
			bFullscreen = false;
			break;
			
		case IDCANCEL:
		default:
			return 0;
		}

		UINT uWidth = GetSystemMetrics(SM_CXSCREEN),
			uHeight = GetSystemMetrics(SM_CYSCREEN);

		if (bFullscreen == false)
		{
			RECT r = { 0, 0, 1280, 720};

			if (AdjustWindowRectEx(&r, dwStyle, FALSE, dwExStyle) == FALSE)
				throw error(__FILE__, __LINE__, HRESULT_FROM_WIN32(GetLastError()));

			uWidth = r.right - r.left;
			uHeight = r.bottom - r.top;
		}

		HWND hWnd = CreateWindowEx(dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle, 0, 0, uWidth, uHeight, nullptr, nullptr, hInst, nullptr);

		if (IsWindow(hWnd) == FALSE)
		{
			throw error(__FILE__, __LINE__, HRESULT_FROM_WIN32(GetLastError()));
		}

		g_Graphics.Initialise(hWnd, bFullscreen);

		g_pHumanPlayer = make_shared<Player>();

		SetTimer(hWnd, TIMER_EVENT_GAMELOGIC, 1000 / 60, [](HWND hWnd, UINT, UINT_PTR, DWORD)
		{
			ControlThread(hWnd, g_pHumanPlayer);
		});

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

		
		message close_msg;
		close_msg.date = high_resolution_clock::now();
		close_msg.msg = messages::terminate;
		close_msg.target = control_thread.get_id();
		AddThreadMessage(close_msg);
		
		return static_cast<int>(msg.message);
	}
	catch (const error& excp)
	{
		std::string str = "An exception has occured.\nFile: " + excp.file + "\nLine: " + to_string(excp.line) + "\nError Code: ";
		str += to_string(_com_error(excp.ec).ErrorMessage());
		str += " (" + to_string(excp.ec) + ")";

		MessageBoxA(nullptr, str.c_str(), "Error!", MB_ICONERROR);

		return -1;
	}
	catch (const string& str)
	{
		MessageBoxA(nullptr, str.c_str(), "Error!", MB_ICONERROR);
		return -1;
	}
}