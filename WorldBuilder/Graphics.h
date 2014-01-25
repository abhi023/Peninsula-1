#pragma once

#include "..\Peninsula\matrix.h"

class IRenderObject
{
public:

};

class IGraphicsLayer
{
public:
	virtual void Initialise(HWND hWnd) = 0;
	virtual void Resize(UINT newWidth, UINT newHeight) = 0;
	virtual void Render() = 0;
};