#pragma once

typedef const wstring& cstr;
typedef wstring str;

struct ui_point
{
	float x;
	float y;

	ui_point()
	{

	}

	template <typename T>
	ui_point(T nx, T ny)
	{
		x = static_cast<float>(nx);
		y = static_cast<float>(ny);
	}
};

struct ui_rect
{
	ui_point ul;
	ui_point lr;

	ui_rect()
	{

	}

	template <typename T>
	ui_rect(T a, T b, T c, T d)
	{
		ul.x = static_cast<float>(a);
		ul.y = static_cast<float>(b);
		lr.x = static_cast<float>(c);
		lr.y = static_cast<float>(d);
	}

	template <typename T>
	ui_rect(T a[4])
	{
		ui_rect(a[0], a[1], a[2], a[3]);
	}

	ui_rect(const ui_point& a, const ui_point& b)
	{
		ul = a;
		lr = b;
	}

	operator D2D1_RECT_F()
	{
		return D2D1::RectF(ul.x, ul.y, lr.x, lr.y);
	}

	float width() { return abs(ul.x - lr.x); }
	float height() { return abs(ul.y - lr.y); }
};

bool inside(const ui_point& pt, const ui_rect& rect);

class ui_element
{
protected:
	CComPtr<ID2D1RenderTarget> m_pRT;

public:
	ui_rect controlRegion;

	void SetRenderTarget(const CComPtr<ID2D1RenderTarget>& rt);

	// Event handlers
	virtual void onCreate() = 0;
	virtual void onLeftClick(unsigned int x, unsigned int y) = 0;
	virtual void onRightClick(unsigned int x, unsigned int y) = 0;
	virtual void onKeyboardEvent(const array<bool, 256>& keys) = 0;

	virtual void resize(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;
	virtual void create(unsigned int x, unsigned int y, unsigned int width, unsigned int height, cstr text) = 0;
	virtual void rename(cstr text) = 0;

	virtual unsigned int get_x() = 0;
	virtual unsigned int get_y() = 0;
	virtual unsigned int get_width() = 0;
	virtual unsigned int get_height() = 0;

	virtual void draw() = 0;
};

class ui_window : 
	public ui_element
{
protected:
	unsigned int m_Overallx;
	unsigned int m_Overally;
	unsigned int m_OverallWidth;
	unsigned int m_OverallHeight;

	unsigned int m_Clientx;
	unsigned int m_Clienty;
	unsigned int m_ClientWidth;
	unsigned int m_ClientHeight;

	CComPtr<ID2D1Brush> m_pBrush_Outline;
	CComPtr<ID2D1Brush> m_pBrush_Background;

	wstring m_strText_Heading;
	unique_ptr<CTextHelper> m_pText_Heading;

	vector<shared_ptr<ui_element>> m_vecParts;

public:

	// user defined handlers
	virtual void onCreate();
	virtual void onLeftClick(unsigned int x, unsigned int y);
	virtual void onRightClick(unsigned int x, unsigned int y);
	virtual void onKeyboardEvent(const array<bool, 256>& keys);

	void create(unsigned int x, unsigned int y, unsigned int width, unsigned int height, cstr text);
	void resize(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	void rename(cstr text);

	void add_part(const shared_ptr<ui_element>& part);

	void draw();
};

class ui_button : 
	public ui_element
{
protected:
	unsigned int m_x;
	unsigned int m_y;
	unsigned int m_width;
	unsigned int m_height;

	CComPtr<ID2D1Brush> m_pBrush_Outline;
	CComPtr<ID2D1Brush> m_pBrush_Area;
	
	wstring m_strText;
	unique_ptr<CTextHelper> m_pText;

public:

	virtual void onLeftClick(unsigned int x, unsigned int y);
	virtual void onRightClick(unsigned int x, unsigned int y);
	virtual void onKeyboardEvent(const array<bool, 256>& keys);

	void create(unsigned int x, unsigned int y, unsigned int width, unsigned int height, cstr text);
	void resize(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	void rename(cstr text);

	void draw();
};

class ui_dropdown : 
	public ui_element
{
public:
	typedef wstring list_item;
	typedef vector<list_item> list_type;

	typedef list_type::size_type index_type;

	typedef list_type::iterator list_itr;
	typedef list_type::const_iterator list_citr;

protected:
	unsigned int m_x;
	unsigned int m_y;
	unsigned int m_width;
	unsigned int m_height;

	CComPtr<ID2D1Brush> m_pBrush_Outline,
		m_pBrush_Body;

	unique_ptr<CTextHelper> m_text_selected,
		m_text_other;

	vector<str> m_vecItems;

	index_type m_selected_item;

	bool m_open;

public:

	void onLeftClick(unsigned int x, unsigned int y);
	void onRightClick(unsigned int x, unsigned int y);
	void onKeyboardEvent(const array<bool, 256>& keys);

	void create(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const vector<str>& text);
	void resize(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	
	// List modifiers
	void add_to_list(cstr text, index_type index = 0);
	bool remove_from_list(cstr text);
	bool remove_from_list(index_type index);

	// Selections
	index_type get_selected_item();
	bool set_selected(cstr text);
	bool set_selected(index_type index);

	// Render
	void draw();
};

class ui_master : 
	public ui_element
{
protected:
	vector<ui_window> m_vecWindows;

public:
	void onLeftClick(unsigned int x, unsigned int y);
	void onRightClick(unsigned int x, unsigned int y);

	void draw();
};