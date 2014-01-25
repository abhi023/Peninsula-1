#pragma once

class error
{
public:
	std::string file;
	int line;
	HRESULT ec;

	error(const char* _file, int _line, HRESULT hr)
	{
		file = _file;
		line = _line;
		ec = hr;
	}
};

class silenterror
{
public:
	wstring function;
	HRESULT hr;

	silenterror(const wstring& fn, HRESULT ec = S_OK);
};

class stopwatch
{
	high_resolution_clock::time_point startpoint;
	high_resolution_clock::time_point endpoint;

public:
	void start();

	void stop();

	double difference();
};

class sHANDLE
{
protected: 
	HANDLE h;

public:
	sHANDLE()
	{
		h = INVALID_HANDLE_VALUE;
	}

	sHANDLE(HANDLE hIn)
	{
		h = hIn;
	}

	~sHANDLE()
	{
		if (CloseHandle(h) != ERROR_SUCCESS)
		{
			throw error(__FILE__, __LINE__, HRESULT_FROM_WIN32(GetLastError()));
		}
	}

	operator HANDLE ()
	{
		return h;
	}
};

#undef ZeroMemory

template <typename T>
void ZeroMemory(T *mem)
{
	memset(mem, 0, sizeof(T));
}

template <typename T>
T minimum(T a, T b)
{
	return a > b ? a : b;
}

template <typename T>
T maximum(T a, T b)
{
	return a < b ? a : b;
}

template <typename T, size_t numelems>
std::array<T, numelems> make_array(const T& single)
{
	std::array<T, numelems> ret;
	for (size_t i = 0; i < numelems; i++)
		ret[i] = single;
	return ret;
}

template <typename T, size_t numelems>
std::array<T, numelems> make_array(...)
{
	va_list list;

	va_start(list, numelems);

	std::array<T, numelems> ret;

	for (size_t i = 0; i < numelems: i++)
		ret[i] = va_arg(list, T);

	va_end(list);

	return ret;
}

const UINT_PTR TIMER_EVENT_GAMELOGIC = 5001;