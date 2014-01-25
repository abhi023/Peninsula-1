#pragma once

class Controls
{
public:
	Controls();
	~Controls();

	static Controls* ctrl;

	static VOID CALLBACK ControlTimerProc(
		_In_ HWND hWnd,
		_In_ UINT uMsg,
		_In_ UINT_PTR idEvent,
		_In_ DWORD dwTime);

	__declspec (align(16)) XMVECTOR pos;
	__declspec (align(16)) XMVECTOR rot;

	UINT mouse_x,
		mouse_y;
};