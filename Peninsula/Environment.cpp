#include "pch.h"
#include "Model.h"
#include "Environment.h"
#include "peninsula.h"
#include "Graphics.h"

CEnvironment::CEnvironment()
{
}

CEnvironment::~CEnvironment()
{
}

//HRESULT CEnvironment::LoadFromFile(const wstring& fn, CComPtr<ID3D11Device> device, CComPtr<ID3D11DeviceContext> context)
//{
//	HRESULT hr = E_NOTIMPL;
//
//	// heightmaps are stored in PNG files
//	CComPtr<IWICImagingFactory> pFactory;
//
//	hr = pFactory.CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER);
//	if (failed(hr))
//		throw error(__FILE__, __LINE__, hr);
//
//	CComPtr<IWICBitmapDecoder> pDecode;
//	hr = pFactory->CreateDecoderFromFilename(fn.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecode.p);
//	if (failed(hr))
//		throw error(__FILE__, __LINE__, hr);
//
//	UINT iFrameCount = 0;
//	hr = pDecode->GetFrameCount(&iFrameCount);
//	if (failed(hr))
//		throw error(__FILE__, __LINE__, hr);
//
//	if (iFrameCount != 1)
//		throw error(__FILE__, __LINE__, hr);
//		
//	CComPtr<IWICBitmapFrameDecode> pFrameDecode;
//	hr = pDecode->GetFrame(0, &pFrameDecode.p);
//	if (failed(hr))
//		throw error(__FILE__, __LINE__, hr);
//
//	UINT uiWidth = 0, uiHeight = 0;
//	hr = pFrameDecode->GetSize(&uiWidth, &uiHeight);
//	if (failed(hr))
//		throw error(__FILE__, __LINE__, hr);
//
//	WICPixelFormatGUID format;
//	hr = pFrameDecode->GetPixelFormat(&format);
//	if (failed(hr))
//		throw error(__FILE__, __LINE__, hr);
//
//	if (format != GUID_WICPixelFormat8bppGray)
//		throw error(__FILE__, __LINE__, hr);
//
//	vector<BYTE> pBuffer(uiWidth * uiHeight);
//	if (pBuffer.size() == 0)
//		throw error(__FILE__, __LINE__, E_OUTOFMEMORY);
//
//	hr = pFrameDecode->CopyPixels(nullptr, uiWidth, pBuffer.size(), &pBuffer[0]);
//	if (failed(hr))
//		throw error(__FILE__, __LINE__, hr);
//
//	vector<Vertex> vecVerts;
//	auto ptr = pBuffer.begin();
//
//	// Load vertex data and texture coordinate data
//	for (BYTE y_coord : pBuffer)
//	{
//		static UINT x_coord;
//		static UINT z_coord;
//
//		Vertex temp;
//		temp.position = XMFLOAT3((float) x_coord, (float) y_coord, (float) z_coord);
//		temp.normal = XMFLOAT3(0.f, 0.f, 0.f);
//		temp.texture = XMFLOAT2((float)x_coord / uiWidth, (float)z_coord / uiHeight);
// 
//		vecVerts.push_back(temp);
//		if (z_coord == uiHeight)
//		{
//			z_coord = 0;
//			x_coord++;
//		}
//		else
//		{
//			z_coord++;
//		}
//	}
//
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
//
//	UINT numTriangles = vecIndices.size() / 3;
//
//	// Per-vertex normals
//	hr = CalculateNormals(&vecVerts[0], vecVerts.size(), vecIndices);
//	if (failed(hr))
//		throw error(__FILE__, __LINE__, hr);;
//
//	// Direct3D11 Buffers
//	D3D11_BUFFER_DESC bd;
//	ZeroMemory(&bd);
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bd.ByteWidth = vecVerts.size() * sizeof(vecVerts[0]);
//	bd.Usage = D3D11_USAGE_DEFAULT;
//
//	D3D11_SUBRESOURCE_DATA sd;
//	ZeroMemory(&sd);
//	sd.pSysMem = &vecVerts[0];
//
//	hr = device->CreateBuffer(&bd, &sd, &m_pVertexBuffer.p);
//	if (failed(hr))
//		throw error(__FILE__, __LINE__, hr);
//
//	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	bd.ByteWidth = vecIndices.size() * sizeof(vecIndices[0]);
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	sd.pSysMem = &vecIndices[0];
//
//	hr = device->CreateBuffer(&bd, &sd, &m_pIndexBuffer.p);
//	if (failed(hr))
//		throw error(__FILE__, __LINE__, hr);
//		
//	nindices = vecIndices.size();
//
//	return hr;
//}

void CEnvironment::LoadFromFile(const wstring& filename, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext)
{

}

void CEnvironment::Render(CComPtr<ID3D11DeviceContext> context)
{
	if (m_pVertexBuffer == nullptr) return;
	if (m_pIndexBuffer == nullptr) return;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer.p, &stride, &offset);

	context->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//context->DrawIndexed(nindices, 0, 0);
}