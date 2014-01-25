#pragma once

#include "Graphics.h"
#include "..\Peninsula\matrix.h"

struct CBProjection
{
	XMMATRIX matProjection;
};

struct CBView
{
	XMMATRIX matView;
};

struct CBWorld
{
	XMMATRIX matWorld;
};

struct CBLights
{
	XMFLOAT4 pos[8];
};

const XMFLOAT4 InvalidLight = XMFLOAT4(0.f, -1.f, 0.f, 0.f);

class ICanRender
{
public:
	XMMATRIX matrix;

	virtual void Render() = 0;
};

class Direct3D11 :
	public IGraphicsLayer
{
protected:
	CComPtr<ID3D11Device> m_pDevice;
	CComPtr<ID3D11DeviceContext> m_pContext;
	CComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	CComPtr<ID3D11Texture2D> m_pDepthStencil;
	CComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

	CComPtr<ID3D11VertexShader> m_pVertexShader;
	CComPtr<ID3D11PixelShader> m_pPixelShader;
	CComPtr<ID3D11InputLayout> m_pInputLayout;

	CComPtr<ID3D11Buffer> m_pCBProjection, m_pCBView, m_pCBWorld, m_pCBLights;
	
	CComPtr<ID3D11SamplerState> m_pSamplerLinear;

	CComPtr<IDXGISwapChain> m_pSwapChain;

	vector<shared_ptr<ICanRender> > m_vecRenderObjects;

	UINT clientWidth, clientHeight;

	CBLights m_lights;

public:
	Direct3D11();
	~Direct3D11();

	void Initialise(HWND hWnd);
	void Resize(UINT newWidth, UINT newHeight);
	void Render();
};