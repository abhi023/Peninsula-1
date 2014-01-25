#include "pch.h"
#include "peninsula.h"
#include "TextHelper.h"
#include "userinterface.h"

//--------------- helper functions
bool inside(const ui_point& prt, const ui_rect& rect)
{
	if (prt.x < rect.ul.x)
		return false;

	if (prt.x > rect.lr.x)
		return false;

	if (prt.y < rect.ul.y)
		return false;

	if (prt.y > rect.lr.y)
		return false;

	return true;
}

//--------------- ui_element class
void ui_element::SetRenderTarget(const CComPtr<ID2D1RenderTarget>& rt)
{
	m_pRT = rt;
}

//--------------- ui_window class
void ui_window::onCreate()
{
}

void ui_window::onLeftClick(unsigned int in_x, unsigned int in_y)
{
	// translate in_x and in_y to (x,y) coordinates relative to window
	unsigned int x = in_x - (m_Overallx + m_Clientx),
		y = in_y - (m_Overally + m_Clienty);

	// iterate through controls until there is a match
	for (const auto& part : m_vecParts)
	{
		if (inside(ui_point(x, y), part->controlRegion))
		{
			part->onLeftClick(x, y);
			return;
		}
	}
}

void ui_window::onRightClick(unsigned int in_x, unsigned int in_y)
{
	// translate in_x and in_y to (x,y) coordinates relative to window
	unsigned int x = in_x - (m_Overallx + m_Clientx),
		y = in_y - (m_Overally + m_Clienty);

	for (const auto& part : m_vecParts)
	{
		if (inside(ui_point(x, y), (part)->controlRegion))
		{
			part->onRightClick(x, y);
			return;
		}
	}
}

void ui_window::onKeyboardEvent(const array<bool, 256>& keys)
{
	if (keys[VK_F1] == true)
		PostQuitMessage(0);
}

void ui_window::create(unsigned int x, unsigned int y, unsigned int width, unsigned int height, cstr text)
{
	if (m_pRT == nullptr)
	{
		throw exception("ui_window::create() -> m_pRT == nullptr");
	}

	HRESULT hr;

	hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Wheat), (ID2D1SolidColorBrush**)&m_pBrush_Background.p);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Brown), (ID2D1SolidColorBrush**)&m_pBrush_Outline.p);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	m_strText_Heading = wstring(text);
	m_pText_Heading = make_unique<CTextHelper>();

	hr = m_pText_Heading->Initialise(L"Tahoma", 18.0f, DWRITE_FONT_WEIGHT_BOLD);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	hr = m_pText_Heading->SetJustification(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	hr = m_pText_Heading->SetColour(D2D1::ColorF(D2D1::ColorF::Azure), m_pRT);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	resize(x, y, width, height);

	onCreate();
}

void ui_window::resize(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	const unsigned int border_thickness = 2;

	m_Overallx = x;
	m_Overally = y;
	m_OverallWidth = width;
	m_OverallHeight = height;

	m_Clientx = border_thickness;
	m_Clienty = 18;
	m_ClientHeight = m_OverallHeight - border_thickness - m_Clienty;
	m_ClientWidth = m_OverallWidth - (2 * border_thickness);

	controlRegion = ui_rect(
		static_cast<float>(x),
		static_cast<float>(y),
		static_cast<float>(x + width),
		static_cast<float>(y + height));
}

void ui_window::rename(cstr text)
{
	m_strText_Heading = wstring(text);
}

void ui_window::add_part(const shared_ptr<ui_element>& part)
{
	m_vecParts.push_back(part);
}

void ui_window::draw()
{
	if (m_pRT == nullptr)
	{
		throw exception("ui_window::draw() -> m_pRT == nullptr");
	}

	// matrix
	D2D1_MATRIX_3X2_F matrix = D2D1::IdentityMatrix();

	// reset Direct2D matrix
	m_pRT->SetTransform(matrix);

	// draw actual window
	D2D1_RECT_F OverallRect = D2D1::RectF(
		static_cast<float>(m_Overallx),
		static_cast<float>(m_Overally),
		static_cast<float>(m_Overallx + m_OverallWidth),
		static_cast<float>(m_Overally + m_OverallHeight));

	m_pRT->FillRectangle(OverallRect, m_pBrush_Outline);

	// text at top of window
	m_pText_Heading->Draw(1.0f, 1.0f, 1.0f, 1.0f, m_strText_Heading, m_pRT);

	// now make everything relative to the actual window we just drew
	D2D1_SIZE_F ClientOffset = D2D1::SizeF(
		static_cast<float>(m_Clientx),
		static_cast<float>(m_Clienty));

	matrix = D2D1::Matrix3x2F::Translation(ClientOffset);
	m_pRT->SetTransform(matrix);

	// draw client rectangle
	D2D1_RECT_F ClientRect = D2D1::RectF(
		static_cast<float>(m_Clientx),
		static_cast<float>(m_Clienty),
		static_cast<float>(m_Clientx + m_ClientWidth),
		static_cast<float>(m_Clienty + m_ClientHeight));

	m_pRT->FillRectangle(ClientRect, m_pBrush_Background);

	// iterate through controls, drawing them
	for (const auto& part : m_vecParts)
	{
		part->draw();
	}
}

//--------------- ui_button class
void ui_button::onLeftClick(unsigned int x, unsigned int y)
{
}

void ui_button::onRightClick(unsigned int x, unsigned int y)
{
}

void ui_button::onKeyboardEvent(const array<bool, 256>& keys)
{
	if (keys[VK_RETURN] == true)
	{
		onLeftClick(0, 0);
	}
}

void ui_button::create(unsigned int x, unsigned int y, unsigned int width, unsigned int height, cstr text)
{
	if (m_pRT == nullptr)
	{
		throw exception("ui_button::create() -> m_pRT == nullptr");
	}

	HRESULT hr;

	hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::AliceBlue), (ID2D1SolidColorBrush**)&m_pBrush_Outline.p);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Coral), (ID2D1SolidColorBrush**)&m_pBrush_Area.p);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	m_strText = wstring(text);
	
	m_pText = make_unique<CTextHelper>();

	hr = m_pText->Initialise();
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	hr = m_pText->SetColour(D2D1::ColorF(D2D1::ColorF::Black), m_pRT);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	hr = m_pText->SetJustification();
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}
}

void ui_button::resize(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;

	controlRegion = ui_rect(m_x, m_y, m_x + m_width, m_y + m_height);
}

void ui_button::rename(cstr text)
{
	m_strText = wstring(text);
}

void ui_button::draw()
{
	D2D1_RECT_F OutlineRect = D2D1::RectF(
		static_cast<float>(m_x),
		static_cast<float>(m_y),
		static_cast<float>(m_x + m_width),
		static_cast<float>(m_y + m_height));

	m_pRT->FillRectangle(OutlineRect, m_pBrush_Area);

	m_pRT->DrawRectangle(OutlineRect, m_pBrush_Outline);

	m_pText->Draw(OutlineRect, m_strText, m_pRT);
}

//--------------- ui_dropdown class
void ui_dropdown::onLeftClick(unsigned int in_x, unsigned int in_y)
{
	unsigned int x = in_x - (unsigned int) controlRegion.ul.x,
		y = in_y - (unsigned int) controlRegion.ul.x;

	if (m_open == false)
	{
		controlRegion = ui_rect(m_x, m_y, m_x + m_width, m_y + ((m_vecItems.size() - 1) * 12));

		m_open = true;
		return;
	}

	if (y <= 12)
	{
		// selected currently selected item

		controlRegion = ui_rect(m_x, m_y, m_x + m_width, m_y + m_height);

		m_open = false;
		return;
	}

	y -= 12;

	index_type itemclicked = 0;

	do {
		itemclicked++;
		y -= 12;
	} while (y > 12);

	m_selected_item = itemclicked;

	// reset to closed control region
	controlRegion = ui_rect(m_x, m_y, m_x + m_width, m_y + m_height);

	m_open = false;
	return;
}

void ui_dropdown::onRightClick(unsigned int x, unsigned int y)
{
}

void ui_dropdown::onKeyboardEvent(const array<bool, 256>& keys)
{
}

void ui_dropdown::create(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const vector<str>& text)
{
	HRESULT hr;

	if (m_pRT == nullptr)
	{
		throw exception("ui_dropdown::create() -> m_pRT == nullptr");
	}

	hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), (ID2D1SolidColorBrush**)&m_pBrush_Outline);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Wheat), (ID2D1SolidColorBrush**)&m_pBrush_Body);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	m_text_selected = make_unique<CTextHelper>();

	hr = m_text_selected->Initialise(L"Tahoma", 12.f, DWRITE_FONT_WEIGHT_SEMI_BOLD);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}
	
	hr = m_text_selected->SetColour(D2D1::ColorF(D2D1::ColorF::Black), m_pRT);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}
	
	hr = m_text_selected->SetJustification();
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	m_text_other = make_unique<CTextHelper>();
	hr = m_text_selected->Initialise();
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	hr = m_text_selected->SetColour(D2D1::ColorF(D2D1::ColorF::Black), m_pRT);
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	hr = m_text_selected->SetJustification();
	if (failed(hr))
	{
		throw error(__FILE__, __LINE__, hr);
	}

	m_selected_item = 0;

	for (auto text_itr : text)
	{
		add_to_list(text_itr);
	}

	resize(x, y, width, height);
}

void ui_dropdown::resize(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;

	controlRegion = ui_rect(m_x, m_y, m_x + m_width, m_y + m_height);
}

void ui_dropdown::add_to_list(cstr text, index_type index)
{
	m_vecItems.push_back(text);

	sort(m_vecItems.begin(), m_vecItems.end());

	m_selected_item = 0;
}

bool ui_dropdown::remove_from_list(cstr text)
{
	try {
		remove(m_vecItems.begin(), m_vecItems.end(), text);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool ui_dropdown::remove_from_list(index_type index)
{
	if (index > m_vecItems.size())
		return false;

	try {
		m_vecItems.erase(m_vecItems.begin() + index);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

ui_dropdown::index_type ui_dropdown::get_selected_item()
{
	return m_selected_item;
}

bool ui_dropdown::set_selected(cstr text)
{
	for (index_type item_itr = 0; item_itr < m_vecItems.size(); item_itr++)
	{
		if (m_vecItems[item_itr] == text)
		{
			return set_selected(item_itr);
		}
	}
	return false;
}

bool ui_dropdown::set_selected(ui_dropdown::index_type index)
{
	if (index > m_vecItems.size())
		return false;

	m_selected_item = index;

	return true;
}

void ui_dropdown::draw()
{
	D2D1_RECT_F CloseBox = D2D1::RectF(
		static_cast<float>(m_x),
		static_cast<float>(m_y),
		static_cast<float>(m_x + m_width),
		static_cast<float>(m_y + m_height));

	m_pRT->FillRectangle(CloseBox, m_pBrush_Body);

	m_pRT->DrawRectangle(CloseBox, m_pBrush_Outline);

	m_text_selected->Draw(CloseBox, m_vecItems[m_selected_item], m_pRT);

	if (m_open == false)
		return;

	D2D1_RECT_F OpenBox = D2D1::RectF(
		static_cast<float>(m_x),
		static_cast<float>(m_y),
		static_cast<float>(m_x + m_width),
		static_cast<float>(m_y + m_height + ((m_vecItems.size() - 1) * 12)));

	m_pRT->FillRectangle(OpenBox, m_pBrush_Body);

	m_pRT->DrawRectangle(OpenBox, m_pBrush_Outline);

	for (index_type i = 0; i < m_vecItems.size(); i++)
	{
		D2D1_RECT_F TextBox = D2D1::RectF(
			static_cast<float>(m_x),
			static_cast<float>(m_y + m_height + (12 * i)),
			static_cast<float>(m_x + m_width),
			static_cast<float>(m_y + m_height + (12 * (i + 1))));

		m_text_selected->Draw(TextBox, m_vecItems[i], m_pRT);
	}
}