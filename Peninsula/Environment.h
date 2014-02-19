#pragma once

template <typename T>
struct point3t
{
	T x;
	T y;
	T z;
};

typedef point3t<float> point3;

namespace EnvironmentConstants
{
	const point3 world_size = { 256, 100, 256 };
	const point3 negative_bound = { 0, 0, 0 };
	const point3 positive_bound = world_size;
};

class CEnvironment :
	public CModel
{
protected:
	size_t n_indices;
public:
	CEnvironment();
	~CEnvironment();

	CComPtr<ID3D11Buffer> m_pVertexBuffer;
	CComPtr<ID3D11Buffer> m_pIndexBuffer;
	
	CComPtr<ID3D11VertexShader> m_pVertexShader;
	CComPtr<ID3D11GeometryShader> m_pGeometryShader;
	CComPtr<ID3D11InputLayout> m_pInputLayout;

	CComPtr<ID3D11ShaderResourceView> m_pTexWorld;
	CComPtr<ID3D11ShaderResourceView> m_pTexHeightmap;

	float m_Grid[256][256];

	void LoadFromFile(const wstring& filename, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);

	float GetY(float x, float z);
	
	void Render(CComPtr<ID3D11DeviceContext> pContext);
};

