#pragma once

class CMesh
{
public:
	CMesh();
	~CMesh();

	CComPtr<ID3D11Buffer> m_pVertexBuffer;
	CComPtr<ID3D11Buffer> m_pIndexBuffer;

	CComPtr<ID3D11Buffer> m_pBoundingVertex;
	CComPtr<ID3D11Buffer> m_pBoundingIndex;

	D3D11_PRIMITIVE_TOPOLOGY mTopology;

	wstring m_strMeshName;

	unsigned int m_uIndices;
	unsigned int m_uMaterialIndex;
};