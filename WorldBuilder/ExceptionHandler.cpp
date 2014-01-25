#include "pch.h"

void ExceptionHandler()
{
	try
	{
		throw;
	}

	catch (const string& str)
	{
		wstring msg(str.begin(), str.end());

		MessageBoxW(nullptr, msg.c_str(), nullptr, MB_ICONERROR);

		terminate();
	}

	catch (const wstring& str)
	{
		MessageBoxW(nullptr, str.c_str(), nullptr, MB_ICONERROR);

		terminate();
	}

	catch (HRESULT hr)
	{
		wstring msg(_com_error(hr).ErrorMessage());

		msg = L"An error has occurred:\n\t" + msg + L" (" + to_wstring(hr) + L")";

		MessageBoxW(nullptr, msg.c_str(), nullptr, MB_ICONERROR);

		terminate();
	} 

	catch (const exception& e)
	{
		string msg = "C++ Exception occurred: \n" + string(e.what()) + "\nProgram will now terminate";

		MessageBoxA(nullptr, msg.c_str(), nullptr, MB_ICONERROR);
	}
}