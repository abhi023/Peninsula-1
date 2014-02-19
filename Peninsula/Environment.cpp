#include "pch.h"
#include "Model.h"
#include "Environment.h"
#include "peninsula.h"
#include "Graphics.h"
#include "quaternion.h"

#include "EnvironmentVertexShader.h"
#include "EnvironmentGeometryShader.h"

struct EnvironmentVertex {
	union {
		float xyz[3];

		struct {
			float x;
			float y;
			float z;
		};
	};
	
	union {
		float uv[2];

		struct {
			float u;
			float v;
		};
	};
};

CEnvironment::CEnvironment()
{
}

CEnvironment::~CEnvironment()
{
}

void CEnvironment::LoadFromFile(const wstring& hmfn, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext)
{
	const unsigned int actualsize = 256;
	const float f_actualsize = static_cast<float>(actualsize);
	const unsigned int division_length = 4;
	const unsigned int num_divs = 256 / 4;

	HRESULT hr = CreateWICTextureFromFile(pDevice, pContext, hmfn.c_str(), nullptr, &m_pTexHeightmap.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	hr = pDevice->CreateVertexShader(EnvironmentVertexShader, sizeof(EnvironmentVertexShader), nullptr, &m_pVertexShader.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	vector<D3D11_INPUT_ELEMENT_DESC> elements =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = pDevice->CreateInputLayout(&elements[0], elements.size(), EnvironmentVertexShader, sizeof(EnvironmentVertexShader), &m_pInputLayout.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	hr = pDevice->CreateGeometryShader(EnvironmentGeometryShader, sizeof(EnvironmentGeometryShader), nullptr, &m_pGeometryShader.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	// 256/4 = 64 x 64 grid
	vector<EnvironmentVertex> envverts(num_divs * num_divs);
	for (float i = 0.f; i < f_actualsize; i += 4.f)
	{
		for (float j = 0.f; j < f_actualsize; j += 4.f)
		{
			EnvironmentVertex ev;
			ev.x = i;
			ev.y = 0;
			ev.z = j;

			ev.u = f_actualsize / i;
			ev.v = f_actualsize / j;

			envverts.push_back(ev);
		}
	}

	//#define XZ(x, z) (x * uiHeight + z)
	//
	//	// Index buffer
	//	vector<unsigned int> vecIndices;
	//	for (UINT x_itr = 0; x_itr < uiWidth; x_itr++)
	//	{
	//		for (UINT z_itr = 0; z_itr < uiHeight; z_itr++)
	//		{
	//			vecIndices.push_back(XZ(x_itr, z_itr));
	//			vecIndices.push_back(XZ(x_itr + 1, z_itr));
	//			vecIndices.push_back(XZ(x_itr, z_itr + 1));
	//
	//			vecIndices.push_back(XZ(x_itr + 1, z_itr + 1));
	//			vecIndices.push_back(XZ(x_itr, z_itr + 1));
	//			vecIndices.push_back(XZ(x_itr + 1, z_itr));
	//		}
	//	}

	auto xz = [actualsize](unsigned int x, unsigned int z)
	{
		return x * actualsize + z;
	};
		
	vector<unsigned int> envindices;
	for (unsigned int x_itr = 0; x_itr < 256; x_itr++)
	{
		for (unsigned int z_itr = 0; z_itr < 64; z_itr++)
		{
			envindices.push_back(xz(x_itr, z_itr));
			envindices.push_back(xz(x_itr, z_itr + 1));
			envindices.push_back(xz(x_itr + 1, z_itr));
		}
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = envverts.size() * sizeof(envverts[0]);
	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd);
	sd.pSysMem = &envverts[0];

	hr = pDevice->CreateBuffer(&bd, &sd, &m_pVertexBuffer.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = envindices.size() * sizeof(envindices[0]);
	sd.pSysMem = &envindices[0];

	hr = pDevice->CreateBuffer(&bd, &sd, &m_pIndexBuffer.p);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);
}

float CEnvironment::GetY(float x, float z)
{
	// establish which triangle we're in
	float loc_x = x - floor(x),
		loc_z = z - floor(x);

	XMVECTOR A;

	if (loc_z > 1.f - loc_x)
	{
		// Upper triangle
		A = XMVectorSet(ceil(x), m_Grid[(size_t)ceil(x)][(size_t)ceil(z)], ceil(z), 0);
	}
	else
	{
		// lower
		A = XMVectorSet(floor(x), m_Grid[(size_t)floor(x)][(size_t)floor(z)], floor(z), 0);
	}

	XMVECTOR B = XMVectorSet(floor(x), m_Grid[(size_t)floor(x)][(size_t)ceil(z)], ceil(z), 0);
	XMVECTOR C = XMVectorSet(ceil(x), m_Grid[(size_t)ceil(x)][(size_t)floor(z)], floor(z), 0);

	XMVECTOR P = XMPlaneFromPoints(A, B, C);

	XMVECTOR veccoord = XMVectorSet(x, 0, z, 1);
	XMVECTOR vtemp = veccoord * P * -1.f;
	
	return (XMVectorGetX(vtemp) + XMVectorGetZ(vtemp) + XMVectorGetW(vtemp)) / XMVectorGetY(P);
}

void CEnvironment::Render(CComPtr<ID3D11DeviceContext> context)
{
	if (m_pVertexBuffer == nullptr) return;
	if (m_pIndexBuffer == nullptr) return;
	if (m_pVertexShader == nullptr) return;
	if (m_pGeometryShader == nullptr) return;
	if (m_pInputLayout == nullptr) return;
	if (m_pTexWorld == nullptr) return;
	if (m_pTexHeightmap == nullptr) return;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// SET INPUT LAYOUT
	context->IASetInputLayout(m_pInputLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// SET VERTEX SHADER
	context->VSSetShader(m_pVertexShader, nullptr, 0);

	// SET GEOMETRY SHADER AND DATA
	context->GSSetShader(m_pGeometryShader, nullptr, 0);
	context->GSSetShaderResources(3, 1, &m_pTexHeightmap.p);

	// SET VERTEX BUFFER
	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer.p, &stride, &offset);

	// SET INDEX BUFFER
	context->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}