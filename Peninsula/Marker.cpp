#include "pch.h"
#include "peninsula.h"
#include "Marker.h"

#include "PixelShader_Mark.h"
#include "VertexShader_Mark.h"

HRESULT Marker::Load(CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext)
{
	// VERTEX SHADER
	HRESULT hr = pDevice->CreateVertexShader(VertexShader_Mark, sizeof(VertexShader_Mark), nullptr, &m_pVertexShader.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// PIXEL SHADER
	hr = pDevice->CreatePixelShader(PixelShader_Mark, sizeof(PixelShader_Mark), nullptr, &m_pPixelShader.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// INPUT LAYOUT
	D3D11_INPUT_ELEMENT_DESC element =
	{
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
	};

	hr = pDevice->CreateInputLayout(&element, 1, VertexShader_Mark, sizeof(VertexShader_Mark), &m_pInputLayout.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// VERTEX BUFFER
	vector<XMFLOAT3> verts =
	{
		{ -1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f },
		{ 0.f, -1.f, 0.f }, { 0.f, 1.f, 0.f },
		{ 0.f, 0.f, -1.f }, { 0.f, 0.f, 1.f },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = verts.size() * sizeof(verts[0]);
	bd.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd);
	sd.pSysMem = &verts[0];

	hr = pDevice->CreateBuffer(&bd, &sd, &m_pVertexBuffer.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	return hr;
}

void Marker::Render(CComPtr<ID3D11DeviceContext> context)
{
	if (context == nullptr) return;
	if (m_pVertexBuffer == nullptr) return;
	if (m_pVertexShader == nullptr) return;
	if (m_pPixelShader == nullptr) return;
	if (m_pInputLayout == nullptr) return;

	context->IASetInputLayout(m_pInputLayout);

	UINT stride = 0, offset = 0;

	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer.p, &stride, &offset);
	
	context->VSSetShader(m_pVertexShader, nullptr, 0);
	context->PSSetShader(m_pPixelShader, nullptr, 0);

	context->Draw(6, 0);
}