#include "pch.h"
#include "peninsula.h"
#include "TextHelper.h"

CComPtr<IDWriteFactory> CTextHelper::pDW = nullptr;

HRESULT CTextHelper::Initialise(const wstring& fontname, float fontsize, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch, const wstring& locale)
{
	HRESULT hr = S_OK;

	if (pDW == nullptr)
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDW));
		if (failed(hr))
			throw error(__FILE__, __LINE__, hr);
	}

	hr = pDW->CreateTextFormat(
		fontname.c_str(),
		nullptr,
		weight,
		style,
		stretch,
		fontsize,
		locale.c_str(),
		&pFormat.p);

	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	return hr;
}

HRESULT CTextHelper::SetJustification(DWRITE_PARAGRAPH_ALIGNMENT paragraph, DWRITE_TEXT_ALIGNMENT text)
{
	HRESULT hr = S_OK;

	if (pFormat == nullptr)
		throw error(__FILE__, __LINE__, E_ABORT);

	hr = pFormat->SetParagraphAlignment(paragraph);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	hr = pFormat->SetTextAlignment(text);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);
	return hr;
}

HRESULT CTextHelper::SetColour(D2D1_COLOR_F colour, ID2D1RenderTarget *pRT)
{
	HRESULT hr = pRT->CreateSolidColorBrush(colour, &pBrush.p);

	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	return hr;
}

void CTextHelper::Draw(float a, float b, float c, float d, const wstring& text, const CComPtr<ID2D1RenderTarget>& pRT)
{
	D2D1_RECT_F boundingrect = D2D1::RectF(a, b, c, d);

	pRT->DrawTextW(text.c_str(), text.length(), pFormat, &boundingrect, pBrush);
}

void CTextHelper::Draw(const D2D1_RECT_F& rect, const wstring& text, const CComPtr<ID2D1RenderTarget>& pRT)
{
	pRT->DrawTextW(text.c_str(), text.length(), pFormat, &rect, pBrush);
}