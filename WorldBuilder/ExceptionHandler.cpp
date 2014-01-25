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

		MessageBox(nullptr, msg.c_str(), nullptr, MB_ICONERROR);

		terminate();
	}

	catch (const wstring& str)
	{
		MessageBox(nullptr, str.c_str(), nullptr, MB_ICONERROR);

		terminate();
	}

	catch (HRESULT hr)
	{
		wstring msg(_com_error(hr).ErrorMessage());

		msg = L"An error has occurred:\n\t" + msg + L" (" + to_wstring(hr) + L")";

		MessageBox(nullptr, msg.c_str(), nullptr, MB_ICONERROR);

		terminate();
	} 

	catch (const exception& e)
	{
		string msg = "C++ Exception occurred: \n" + string(e.what()) + "\nProgram will now terminate";

		wstring wmsg(msg.begin(), msg.end());

		MessageBox(nullptr, wmsg.c_str(), nullptr, MB_ICONERROR);
	}
}