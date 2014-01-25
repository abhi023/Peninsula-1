#pragma once

// Windows
#include <Windows.h>
#include <windowsx.h>

// COM
#include <atlbase.h>
#include <comdef.h>
#include <Initguid.h>

// Direct3D
#include <d3d11.h>
#include <DirectXMath.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

// Direct2D
#include <d2d1.h>

// DirectWrite
#include <dwrite.h>

// C++ / STL
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <memory>
#include <chrono>
#include <map>
#include <bitset>
#include <fstream>
#include <algorithm>
#include <thread>
#include <complex>
#include <type_traits>
#include <cstdlib>

// Libraries
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using namespace std;
using namespace DirectX;

void ExceptionHandler();