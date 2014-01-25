#include "pch.h"

#include "Graphics.h"
#include "Direct3D11.h"

#include "VertexShader.h"
#include "PixelShader.h"

Direct3D11::Direct3D11()
{
}


Direct3D11::~Direct3D11()
{
}

void Direct3D11::Initialise(HWND hWnd)
{
	vector<D3D_DRIVER_TYPE> driverTypes =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	vector<D3D_FEATURE_LEVEL> featureLevels =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
	};

	UINT driverFlags =
#ifdef _DEBUG
		D3D11_CREATE_DEVICE_DEBUG
#else
		0
#endif
		;

	RECT rc;
	GetClientRect(hWnd, &rc);
	clientWidth = rc.right - rc.left;
	clientHeight = rc.bottom - rc.top;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.Height = clientHeight;
	sd.BufferDesc.Width = clientWidth;
	sd.BufferDesc.RefreshRate = { 60, 1 };
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc = { 1, 0 };
	sd.Windowed = TRUE;

	HRESULT hr = S_OK;

	for (auto dt : driverTypes)
	{
		hr = D3D11CreateDeviceAndSwapChain(nullptr, dt, nullptr, driverFlags, &featureLevels[0], featureLevels.size(), D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice, nullptr, &m_pContext);

		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		throw hr;

	CComPtr<ID3D11Texture2D> pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer.p));
	if (FAILED(hr))
		throw hr;

	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
	if (FAILED(hr))
		throw hr;

	D3D11_TEXTURE2D_DESC dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Width = clientWidth;
	dd.Height = clientHeight;
	dd.MipLevels = 1;
	dd.ArraySize = 1;
	dd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dd.SampleDesc = { 1, 0 };
	dd.Usage = D3D11_USAGE_DEFAULT;
	dd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dd.CPUAccessFlags = 0;
	dd.MiscFlags = 0;
	hr = m_pDevice->CreateTexture2D(&dd, nullptr, &m_pDepthStencil);
	if (FAILED(hr))
		throw hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = dd.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
	if (FAILED(hr))
		throw hr;

	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView.p, m_pDepthStencilView);

	hr = m_pDevice->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &m_pVertexShader.p);
	if (FAILED(hr))
		throw hr;

	vector < D3D11_INPUT_ELEMENT_DESC> layout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_pDevice->CreateInputLayout(&layout[0], layout.size(), VertexShader, sizeof(VertexShader), &m_pInputLayout);
	if (FAILED(hr))
		throw hr;

	hr = m_pDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &m_pPixelShader.p);
	if (FAILED(hr))
		throw hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	bd.ByteWidth = sizeof(CBProjection);
	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	hr = m_pDevice->CreateBuffer(&bd, nullptr, &m_pCBProjection);
	if (FAILED(hr))
		throw hr;

	bd.ByteWidth = sizeof(CBView);
	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	hr = m_pDevice->CreateBuffer(&bd, nullptr, &m_pCBView);
	if (FAILED(hr))
		throw hr;

	bd.ByteWidth = sizeof(CBWorld);
	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	hr = m_pDevice->CreateBuffer(&bd, nullptr, &m_pCBWorld);
	if (FAILED(hr))
		throw hr;

	bd.ByteWidth = sizeof(CBLights);
	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	hr = m_pDevice->CreateBuffer(&bd, nullptr, &m_pCBLights);
	if (FAILED(hr))
		throw hr;

	XMVECTOR at = XMVectorSet(128.f, 0.f, 128.f, 0.f);
	XMVECTOR eye = XMVectorSet(0.f, 128.f, 0.f, 0.f);
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMMATRIX mView = XMMatrixLookAtLH(eye, at, up);

	CBView view = { XMMatrixTranspose(mView) };
	m_pContext->UpdateSubresource(m_pCBView, 0, nullptr, &view, 0, 0);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
	if (FAILED(hr))
		throw hr;

	for (unsigned int i = 0; i < 8; i++)
	{
		m_lights.pos[i] = InvalidLight;
	}
}

void Direct3D11::Resize(UINT width, UINT height)
{
	XMMATRIX mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (float)height, 0.01f, 10000.f);

	CBProjection projection = { XMMatrixTranspose(mProjection) };

	m_pContext->UpdateSubresource(m_pCBProjection, 0, nullptr, &projection, 0, 0);

	m_pContext->OMSetRenderTargets(0, nullptr, nullptr);

	// release references to swap chain buffers
	m_pRenderTargetView.Release();

	HRESULT hr = S_OK;

	hr = m_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
		throw hr;

	CComPtr<ID3D11Texture2D> pBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBuffer.p));
	if (FAILED(hr))
		throw hr;

	hr = m_pDevice->CreateRenderTargetView(pBuffer, nullptr, &m_pRenderTargetView.p);
	if (FAILED(hr))
		throw hr;

	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView.p, nullptr);

	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_pContext->RSSetViewports(1, &vp);
}

void Direct3D11::Render()
{
	// CLEAR BACK BUFFER
	float ClearColour[4] = { 0.f, 0.f, 0.f, 1.f };
	m_pContext->ClearRenderTargetView(m_pRenderTargetView, ClearColour);

	// CLEAR DEPTH BUFFER
	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

	// SET INPUT LAYOUT
	m_pContext->IASetInputLayout(m_pInputLayout);

	// SET VERTEX SHADER AND CONSTANT BUFFERS
	m_pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &m_pCBProjection.p);
	m_pContext->VSSetConstantBuffers(1, 1, &m_pCBView.p);
	m_pContext->VSSetConstantBuffers(2, 1, &m_pCBWorld.p);

	// SET PIXEL SHADER AND CONSTANT BUFFERS
	m_pContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pContext->PSSetSamplers(0, 1, &m_pSamplerLinear.p);

	static float t = 0.f;
	t += 0.05f;

	for (size_t itr = 0; itr < m_vecRenderObjects.size(); itr++)
	{
		CBWorld world = { XMMatrixTranspose(m_vecRenderObjects[itr]->matrix) };
		m_pContext->UpdateSubresource(m_pCBWorld, 0, nullptr, &world, 0, 0);

		m_vecRenderObjects[itr]->Render();
	}

	m_pSwapChain->Present(1, 0);
}