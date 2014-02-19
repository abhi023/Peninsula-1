#include "pch.h"
#include "peninsula.h"
#include "Graphics.h"
#include "Model.h"
#include "TextHelper.h"
#include "Control.h"
#include "Environment.h"
#include "GameEntity.h"

#include "VertexShader.h"
#include "PixelShader.h"

//extern shared_ptr<Controls> g_Controls;
extern shared_ptr<Player> g_pHumanPlayer;
extern array<bool, 256> bKeys;

CGraphics::CGraphics() : m_hInstance(nullptr), m_hWnd(nullptr),	m_mView(XMMatrixIdentity()), m_mProjection(XMMatrixIdentity()),	m_pd2dFactory(nullptr), m_pd2dBackBufferRT(nullptr), m_pDebugInfoText(nullptr),	m_DriverType(D3D_DRIVER_TYPE_NULL), m_FeatureLevel(D3D_FEATURE_LEVEL_11_1),	m_pd3dDevice(nullptr), m_pd3dImmediateContext(nullptr), m_pRenderTargetView(nullptr), m_pDepthStencil(nullptr), m_pDepthStencilView(nullptr), m_pVertexShader(nullptr), m_pPixelShader(nullptr), m_pInputLayout(nullptr), m_pCBView(nullptr), m_pCBProjection(nullptr), m_pSamplerLinear(nullptr), m_pSwapChain(nullptr), m_pdxgiDevice(nullptr), m_pdxgiAdapter(nullptr), m_pdxgiFactory(nullptr), m_bFullscreen(false), m_FPS(0), display_debug_info(true)
{
	OutputDebugStringW(L"CGraphics created.\n");
}

CGraphics::~CGraphics()
{
	CMaterial::pCBMaterial.Release();
	OutputDebugStringW(L"CGraphics destroyed.\n");
}


HRESULT CGraphics::Initialise(HWND hWnd, bool bFullscreen)
{
	m_hWnd = hWnd;
	m_hInstance = GetModuleHandle(nullptr);
	m_bFullscreen = bFullscreen;

	m_DriverType = D3D_DRIVER_TYPE_NULL;
	m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	UINT width = rc.right - rc.left,
		height = rc.bottom - rc.top;

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	vector<D3D_DRIVER_TYPE> driverTypes 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	vector<D3D_FEATURE_LEVEL> featureLevels 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd);
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate = { 60, 1 };
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = bFullscreen == true ? FALSE : TRUE;

	HRESULT hr = S_OK;

	for (D3D_DRIVER_TYPE t : driverTypes)
	{
		m_DriverType = t;

		hr = D3D11CreateDeviceAndSwapChain(nullptr, m_DriverType, nullptr, createDeviceFlags, &featureLevels[0], featureLevels.size(), 
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_FeatureLevel, &m_pd3dImmediateContext);
		
		if (succeeded(hr))
			break;
	}

	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	m_MatrixStack.m_pContext = m_pd3dImmediateContext;
	
	CComPtr<ID3D11Texture2D> pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// check multithreading capabilities
	D3D11_FEATURE_DATA_THREADING threadStruct;
	ZeroMemory(&threadStruct);
	m_pd3dDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threadStruct, sizeof(threadStruct));
	if (threadStruct.DriverConcurrentCreates == FALSE)
	{
		OutputDebugString(L"Multithreaded resource creation is not supported by the graphics hardware in this machine. This may result in slower loading times.\n");
	}

	if (threadStruct.DriverCommandLists == FALSE)
	{
		OutputDebugString(L"Multithreaded rendering is not supported by the graphics hardware in this machine. This may result in reduced framerates and graphics capabilities.\n");
	}

	// create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth);
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc = { 1, 0 }; // count, quality
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &m_pDepthStencil);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV);
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	m_pd3dImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView.p, m_pDepthStencilView);

	// create vertex shader
	hr = m_pd3dDevice->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &m_pVertexShader);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// create input layout
	vector<D3D11_INPUT_ELEMENT_DESC> layout 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_pd3dDevice->CreateInputLayout(&layout[0], layout.size(), VertexShader, sizeof(VertexShader), &m_pInputLayout);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// set input layout
	m_pd3dImmediateContext->IASetInputLayout(m_pInputLayout);

	// create pixel shader
	hr = m_pd3dDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &m_pPixelShader);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// create constant buffers
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	bd.ByteWidth = sizeof(CBView);
	bd.CPUAccessFlags = /*D3D11_CPU_ACCESS_WRITE*/ 0;
	bd.Usage = /*D3D11_USAGE_DYNAMIC*/ D3D11_USAGE_DEFAULT;
	hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &m_pCBView);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	bd.ByteWidth = sizeof(CBProjection);
	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &m_pCBProjection);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	bd.ByteWidth = sizeof(CBWorld);
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &m_MatrixStack.m_pCBWorld);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	bd.ByteWidth = sizeof(CBMaterial);
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &CMaterial::pCBMaterial);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// initialise view matrix
	XMVECTOR at = XMVectorSet(0.f, 3.f, -6.f, 0.f);
	XMVECTOR eye = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	m_mView = XMMatrixLookAtLH(eye, at, up);
	CBView cbview;
	ZeroMemory(&cbview);
	cbview.mView = XMMatrixTranspose(m_mView);

	m_pd3dImmediateContext->UpdateSubresource(m_pCBView, 0, nullptr, &cbview, 0, 0);

	// initialise cube
	shared_ptr<CModel> cube = make_shared<CModel>();
	cube->LoadFromFile(L"assets\\models\\newbox.3ds", m_pd3dDevice, m_pd3dImmediateContext);
	models.push_back(cube);

	// sphere
	shared_ptr<CModel> filemodel = make_shared<CModel>();
	filemodel->LoadFromFile(L"assets\\models\\sphere.3ds", m_pd3dDevice, m_pd3dImmediateContext);
	models.push_back(filemodel);

	// boardroom
	shared_ptr<CModel> boardroom = make_shared<CModel>();
	boardroom->LoadFromFile(L"assets\\environment\\boardroom.3ds", m_pd3dDevice, m_pd3dImmediateContext);
	models.push_back(boardroom);

	//// heightmap
	//shared_ptr<CModel> heightmap = make_shared<CEnvironment>();
	//heightmap->LoadFromFile(L"assets\\environment\\heightmap.png", m_pd3dDevice, m_pd3dImmediateContext);
	//models.push_back(heightmap);

	// create sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc);
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// initialise projection matrix
	m_mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (float)height, 0.01f, 100000.f);

	CBProjection cb;
	cb.mProjection = XMMatrixTranspose(m_mProjection);

	m_pd3dImmediateContext->UpdateSubresource(m_pCBProjection, 0, nullptr, &cb, 0, 0);

	// set viewport
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pd3dImmediateContext->RSSetViewports(1, &vp);

	// DIRECT2D
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pd2dFactory.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	float dpiX, dpiY;
	m_pd2dFactory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpiX, dpiY);

	CComPtr<IDXGISurface> pSurfaceBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pSurfaceBackBuffer));
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(pSurfaceBackBuffer, &props, &m_pd2dBackBufferRT.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// FPS text
	m_pDebugInfoText = make_unique<CTextHelper>();
	m_pDebugInfoText->Initialise();
	m_pDebugInfoText->SetColour(D2D1::ColorF(D2D1::ColorF::Yellow), m_pd2dBackBufferRT);
	m_pDebugInfoText->SetJustification(DWRITE_PARAGRAPH_ALIGNMENT_NEAR, DWRITE_TEXT_ALIGNMENT_LEADING);

	m_pMarker = make_unique<Marker>();
	m_pMarker->Load(m_pd3dDevice, m_pd3dImmediateContext);

	return hr;
}

HRESULT CGraphics::OnResize(UINT newwidth, UINT newheight)
{
	HRESULT hr = S_OK;



	static bool resizebuffer = false;

	if (resizebuffer == true)
	{
		DXGI_SWAP_CHAIN_DESC sd;
		hr = m_pSwapChain->GetDesc(&sd);
		if (failed(hr))
			throw error(__FILE__, __LINE__, hr);

		hr = m_pSwapChain->ResizeBuffers(1, newwidth, newheight, sd.BufferDesc.Format, sd.Flags);
		if (failed(hr))
			throw error(__FILE__, __LINE__, hr);
	}

	resizebuffer = true;
	return S_OK;
}

void CGraphics::ToggleDebugInfo()
{
	display_debug_info = display_debug_info == true ? false : true;
}

void CGraphics::Render()
{
	HRESULT hr;

	frameratetimer.start();

	// CLEAR BACK BUFFER
	float ClearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView, ClearColour);

	// CLEAR DEPTH BUFFER
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// CLEAR MATRIX STACK
	m_MatrixStack.flush();

	// SET INPUT LAYOUT
	m_pd3dImmediateContext->IASetInputLayout(m_pInputLayout);
		
	// SET VERTEX SHADER AND CONSTANT BUFFERS
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, &m_pCBView.p);
	m_pd3dImmediateContext->VSSetConstantBuffers(1, 1, &m_pCBProjection.p);
	m_pd3dImmediateContext->VSSetConstantBuffers(2, 1, &m_MatrixStack.m_pCBWorld.p);

	// SET PIXEL SHADER AND CONSTANT BUFFERS
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pd3dImmediateContext->PSSetConstantBuffers(2, 1, &m_MatrixStack.m_pCBWorld.p);
	m_pd3dImmediateContext->PSSetConstantBuffers(3, 1, &CMaterial::pCBMaterial.p);
	m_pd3dImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear.p);

	static float t = 0.f;
	t += 0.05f;

	XMVECTOR translation = _mm_load_ps(g_pHumanPlayer->xyz);
	translation = _mm_mul_ps(translation, _mm_set_ps1(-1.f));

	XMMATRIX pos = XMMatrixTranslationFromVector(translation);

	m_MatrixStack.push_last(pos);

	m_MatrixStack.push(XMMatrixTranslation(-0.5f, 0.f, -0.5f));
		m_MatrixStack.push(XMMatrixRotationY(t));
			m_MatrixStack.push(XMMatrixTranslation(2.f, 0.f, 0.f));
				models[0]->Render(m_pd3dImmediateContext);
			m_MatrixStack.pop();
		m_MatrixStack.pop();
	m_MatrixStack.pop();
	
	m_MatrixStack.push(XMMatrixRotationY(-t));
		models[1]->Render(m_pd3dImmediateContext);
	m_MatrixStack.pop();

	//models[2]->Render(m_pd3dImmediateContext);

	m_pMarker->Render(m_pd3dImmediateContext);

	// PRINT FPS IN TOP LEFT CORNER
	if (m_pd2dBackBufferRT && display_debug_info == true)
	{
		wstringstream fpsoutput;

		if (m_FPS == INFINITY)
			fpsoutput << L">1000 FPS"; //L"\u221E FPS";
		else
			fpsoutput << m_FPS << L" FPS";

		fpsoutput.precision(4);
		fpsoutput << L" (" << 1000 / m_FPS << L"ms)";
		
		wstring wstr = fpsoutput.str();

		wstring posoutput = 
			L"x: " + to_wstring(g_pHumanPlayer->x) +
			L"\ty: " + to_wstring(g_pHumanPlayer->y) +
			L"\tz: " + to_wstring(g_pHumanPlayer->z) +

			L"\nroll: " + to_wstring(g_pHumanPlayer->roll) +
			L"\tpitch: " + to_wstring(g_pHumanPlayer->pitch) +
			L"\tyaw: " + to_wstring(g_pHumanPlayer->yaw);

		m_pd2dBackBufferRT->BeginDraw();

		m_pDebugInfoText->Draw(0, 0, m_pd2dBackBufferRT->GetSize().width, m_pd2dBackBufferRT->GetSize().height, wstr, m_pd2dBackBufferRT);
		m_pDebugInfoText->Draw(0, 15, m_pd2dBackBufferRT->GetSize().width, m_pd2dBackBufferRT->GetSize().width, posoutput, m_pd2dBackBufferRT);

		hr = m_pd2dBackBufferRT->EndDraw();
		if (failed(hr))
			throw error(__FILE__, __LINE__, hr);
	}
	
	frameratetimer.stop();
	// only recalculate framerate every 100 frames
	static uint32_t framecounter = 0;
	framecounter++;
	if (framecounter == 100)
	{
		framecounter = 0;
		m_FPS = static_cast<double>(1 / frameratetimer.difference());
	}

	// PRESENT
	m_pSwapChain->Present(1, 0);
}
