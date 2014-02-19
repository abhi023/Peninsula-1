#include "pch.h"
#include "Control.h"
#include "peninsula.h"
#include "Graphics.h"
#include "thread_messages.h"
#include "GameEntity.h"

unsigned int mouse_x, mouse_y;

void ControlThread(HWND hWnd, shared_ptr<Player> pHumanPlayer)
{
	if (hWnd == nullptr)
		return;

	if (pHumanPlayer == nullptr)
		return;

	// retrieve window size
	WINDOWINFO winfo;
	ZeroMemory(&winfo);
	winfo.cbSize = sizeof(winfo);
	GetWindowInfo(hWnd, &winfo);
		
	UINT width = winfo.rcClient.right - winfo.rcClient.left,
		height = winfo.rcClient.bottom - winfo.rcClient.top;
	
	UINT centre_x = (width / 2) + winfo.rcClient.left,
		centre_y = (height / 2) + winfo.rcClient.top;
	
	INT cMouseX = mouse_x - centre_x,
		cMouseY = mouse_y - centre_y;
	
	INT mouse_magnitude = (INT) sqrt(pow(cMouseX, 2) + pow(cMouseY, 2));
	
	cMouseX /= mouse_magnitude;
	cMouseY /= mouse_magnitude;
	
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}
	
	if (GetAsyncKeyState(VK_SPACE))
	{
		pHumanPlayer->y += 1.f;
	}
	
	if (GetAsyncKeyState(VK_CONTROL))
	{
		pHumanPlayer->y -= 1.f;
	}
	
	if (GetAsyncKeyState('W'))
	{
		pHumanPlayer->z += 0.5f;
	}
	
	if (GetAsyncKeyState('S'))
	{
		pHumanPlayer->z -= 0.5f;
	}
	
	if (GetAsyncKeyState('A'))
	{
		pHumanPlayer->x -= 0.5f;
	}
	
	if (GetAsyncKeyState('D'))
	{
		pHumanPlayer->x += 0.5f;
	}
}

//Controls::Controls(HWND inWnd) : hWnd(inWnd), pos(XMVectorSet(0.f, 0.f, 0.f, 1.f)), rot(XMVectorSet(0.f, 0.f, 0.f, 0.f)), mouse_x(0), mouse_y(0)
//{
//	ctrl = this;
//	OutputDebugStringW(L"Controls class constructed\n");
//
//	ctrlthread = thread(
//	[this]()
//	{
//		high_resolution_clock::time_point start = high_resolution_clock::now();
//
//		while (true)
//		{
//			do
//			{
//				high_resolution_clock::time_point tp = high_resolution_clock::now();
//
//				double difference = duration_cast<duration<double>>(tp - start).count();
//
//				const double fps60 = 1.0 / 60.0;
//
//				if (difference < fps60)
//				{
//					this_thread::yield();
//				}
//				else
//				{
//					break;
//				}
//			} while (true);
//			
//			start = high_resolution_clock::now();
//
//			//if (GetActiveWindow() != hWnd)
//			//	continue;
//
//			// retrieve window size
//			WINDOWINFO winfo;
//			ZeroMemory(&winfo);
//			winfo.cbSize = sizeof(winfo);
//			GetWindowInfo(hWnd, &winfo);
//
//			UINT width = winfo.rcClient.right - winfo.rcClient.left,
//				height = winfo.rcClient.bottom - winfo.rcClient.top;
//
//			UINT centre_x = (width / 2) + winfo.rcClient.left,
//				centre_y = (height / 2) + winfo.rcClient.top;
//
//			INT cMouseX = mouse_x - centre_x,
//				cMouseY = mouse_y - centre_y;
//
//			INT mouse_magnitude = (INT)sqrt(pow(cMouseX, 2) + pow(cMouseY, 2));
//
//			cMouseX /= mouse_magnitude;
//			cMouseY /= mouse_magnitude;
//
//			float px = 0.f;
//			XMVectorGetXPtr(&px, ctrl->rot);
//			px += cMouseY;
//
//			//SetCursorPos(centre_x, centre_y);
//
//			if (GetAsyncKeyState(VK_ESCAPE))
//			{
//				PostQuitMessage(0);
//			}
//
//			XMVECTOR cpos = XMVectorZero();
//
//			if (GetAsyncKeyState(VK_SPACE))
//			{
//				cpos = XMVectorAdd(cpos, XMVectorSet(0.f, 1.f, 0.f, 0.f));
//			}
//
//			if (GetAsyncKeyState(VK_CONTROL))
//			{
//				cpos = XMVectorAdd(cpos, XMVectorSet(0.f, -1.f, 0.f, 0.f));
//			}
//
//			if (GetAsyncKeyState('W'))
//			{
//				cpos = XMVectorAdd(cpos, XMVectorSet(0.f, 0.f, 0.5f, 0.f));
//			}
//
//			if (GetAsyncKeyState('S'))
//			{
//				cpos = XMVectorAdd(cpos, XMVectorSet(0.f, 0.f, -0.5f, 0.f));
//			}
//
//			if (GetAsyncKeyState('A'))
//			{
//				cpos = XMVectorAdd(cpos, XMVectorSet(-0.5f, 0.f, 0.f, 0.f));
//			}
//
//			if (GetAsyncKeyState('D'))
//			{
//				cpos = XMVectorAdd(cpos, XMVectorSet(0.5f, 0.f, 0.f, 0.f));
//			}
//
//			pos = XMVectorAdd(pos, cpos);
//		}
//
//		OutputDebugString(L"Leaving control loop");
//	});
//}
//
//Controls::~Controls()
//{
//	if (ctrlthread.joinable())
//		ctrlthread.join();
//
//	OutputDebugStringW(L"Controls class destroyed\n");
//}

//Controls* Controls::ctrl = nullptr;
//extern CGraphics g_Graphics;
//VOID CALLBACK Controls::ControlTimerProc(
//	_In_ HWND hWnd,
//	_In_ UINT uMsg,
//	_In_ UINT_PTR idEvent,
//	_In_ DWORD dwTime)
//{
//	if (ctrl == nullptr)
//		throw error(__FILE__, __LINE__, E_INVALIDARG);
//
//	if (idEvent != TIMER_EVENT_GAMELOGIC)
//		return;
//
//	if (GetActiveWindow() != hWnd)
//		return;
//
//	// retrieve window size
//	WINDOWINFO winfo;
//	ZeroMemory(&winfo);
//	winfo.cbSize = sizeof(winfo);
//	GetWindowInfo(hWnd, &winfo);
//	
//	UINT width = winfo.rcClient.right - winfo.rcClient.left,
//		height = winfo.rcClient.bottom - winfo.rcClient.top;
//
//	UINT centre_x = (width / 2) + winfo.rcClient.left,
//		centre_y = (height / 2) + winfo.rcClient.top;
//
//	INT cMouseX = ctrl->mouse_x - centre_x,
//		cMouseY = ctrl->mouse_y - centre_y;
//
//	INT mouse_magnitude = (INT) sqrt(pow(cMouseX, 2) + pow(cMouseY, 2));
//
//	cMouseX /= mouse_magnitude;
//	cMouseY /= mouse_magnitude;
//
//	float px = 0.f;
//	XMVectorGetXPtr(&px, ctrl->rot);
//	px += cMouseY;
//
//	//SetCursorPos(centre_x, centre_y);
//
//	if (GetAsyncKeyState(VK_F11))
//	{
//		g_Graphics.ToggleDebugInfo();
//	}
//
//	if (GetAsyncKeyState(VK_ESCAPE))
//	{
//		PostQuitMessage(0);
//	}
//
//	XMVECTOR cpos = XMVectorZero();
//
//	if (GetAsyncKeyState(VK_SPACE))
//	{
//		cpos = XMVectorAdd(cpos, XMVectorSet(0.f, 1.f, 0.f, 0.f));
//	}
//
//	if (GetAsyncKeyState(VK_CONTROL))
//	{
//		cpos = XMVectorAdd(cpos, XMVectorSet(0.f, -1.f, 0.f, 0.f));
//	}
//
//	if (GetAsyncKeyState('W'))
//	{
//		cpos = XMVectorAdd(cpos, XMVectorSet(0.f, 0.f, 0.5f, 0.f));
//	}
//
//	if (GetAsyncKeyState('S'))
//	{
//		cpos = XMVectorAdd(cpos, XMVectorSet(0.f, 0.f, -0.5f, 0.f));
//	}
//
//	if (GetAsyncKeyState('A'))
//	{
//		cpos = XMVectorAdd(cpos, XMVectorSet(-0.5f, 0.f, 0.f, 0.f));
//	}
//
//	if (GetAsyncKeyState('D'))
//	{
//		cpos = XMVectorAdd(cpos, XMVectorSet(0.5f, 0.f, 0.f, 0.f));
//	}
//
//	ctrl->pos = XMVectorAdd(ctrl->pos, cpos);
//}