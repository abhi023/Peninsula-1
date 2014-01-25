#pragma once

#include "pch.h"

class CTextHelper
{
public:
	static CComPtr<IDWriteFactory> pDW;

	CComPtr<IDWriteTextFormat> pFormat;
	CComPtr<ID2D1SolidColorBrush> pBrush;

	HRESULT Initialise(const wstring& fontname = L"Tahoma", float fontsize = 12.0f, DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH = DWRITE_FONT_STRETCH_NORMAL, const wstring& locale = L"en-us");

	HRESULT SetJustification(DWRITE_PARAGRAPH_ALIGNMENT paragraph = DWRITE_PARAGRAPH_ALIGNMENT_CENTER, DWRITE_TEXT_ALIGNMENT text = DWRITE_TEXT_ALIGNMENT_CENTER);

	HRESULT SetColour(D2D1_COLOR_F colour, ID2D1RenderTarget *pRT);

	void Draw(float xUL, float yUL, float xLR, float yLR, const wstring& text, const CComPtr<ID2D1RenderTarget>& pRT);
	void Draw(const D2D1_RECT_F& rect, const wstring& text, const CComPtr<ID2D1RenderTarget>& pRT);
};