#pragma once

#include "Model.h"
#include "peninsula.h"
#include "TextHelper.h"

#include "Marker.h"
#include "MatrixStack.h"

// structures
struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texture;
};

struct CBView
{
	XMMATRIX mView;
};

struct CBProjection
{
	XMMATRIX mProjection;
};

struct CBWorld
{
	XMMATRIX mWorld;
};

class CGraphics
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	vector<shared_ptr<CModel>> models;
	
	__declspec(align(16)) XMMATRIX m_mView, m_mProjection;

	// DIRECT2D 
	CComPtr<ID2D1Factory> m_pd2dFactory;
	CComPtr<ID2D1RenderTarget> m_pd2dBackBufferRT;

	// TEXT INSTANCES
	unique_ptr<CTextHelper> m_pDebugInfoText;

	unique_ptr<Marker> m_pMarker;

	// DIRECT3D
	D3D_DRIVER_TYPE m_DriverType;
	D3D_FEATURE_LEVEL m_FeatureLevel;

	CComPtr<ID3D11Device> m_pd3dDevice;
	CComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;
	CComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	CComPtr<ID3D11Texture2D> m_pDepthStencil;
	CComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	CComPtr<ID3D11VertexShader> m_pVertexShader;
	CComPtr<ID3D11PixelShader> m_pPixelShader;
	CComPtr<ID3D11InputLayout> m_pInputLayout;

	CComPtr<ID3D11Buffer> m_pCBView;
	CComPtr<ID3D11Buffer> m_pCBProjection;

	CComPtr<ID3D11SamplerState> m_pSamplerLinear;

	// DXGI
	CComPtr<IDXGISwapChain> m_pSwapChain;
	CComPtr<IDXGIDevice> m_pdxgiDevice;
	CComPtr<IDXGIAdapter> m_pdxgiAdapter;
	CComPtr<IDXGIFactory> m_pdxgiFactory;

	stopwatch frameratetimer;

	bool m_bFullscreen;

	double m_FPS;
	bool display_debug_info;
	
	MatrixStack m_MatrixStack;

public:
	CGraphics();
	~CGraphics();

	HRESULT Initialise(HWND hWnd, bool bFullscreen);

	HRESULT InitialiseResources();

	HRESULT OnResize(UINT width, UINT height);

	void ToggleDebugInfo();

	void Render();
};