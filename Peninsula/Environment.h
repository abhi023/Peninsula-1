#pragma once

template <typename T>
struct point3
{
	T x;
	T y;
	T z;
};

namespace EnvironmentConstants
{
	const point3<float> world_size = { 1000, 100, 1000 };
	const point3<float> negative_bound = { 0, 0, 0 };
	const point3<float> positive_bound = world_size;
};

class CEnvironment :
	public CModel
{
public:
	CEnvironment();
	~CEnvironment();

	CComPtr<ID3D11Buffer> m_pVertexBuffer;
	CComPtr<ID3D11Buffer> m_pIndexBuffer;

	CComPtr<ID3D11ShaderResourceView> m_pTexGround;
	CComPtr<ID3D11ShaderResourceView> m_pTextHeightmap;
	CComPtr<ID3D11ShaderResourceView> m_pTexSkybox;

	void LoadFromFile(const wstring& filename, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);
	
	void Render(CComPtr<ID3D11DeviceContext> pContext);
};

