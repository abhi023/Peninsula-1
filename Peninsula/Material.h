#pragma once

struct CBMaterial
{
	XMFLOAT4 diffuse,
	transparent;
};

class CMaterial
{
public:
	static CComPtr<ID3D11Buffer> pCBMaterial;

	CMaterial();
	~CMaterial();

	wstring name;

	CComPtr<ID3D11ShaderResourceView>
		m_pTextureDiffuse,
		m_pTextureNormals,
		m_pTextureOpacity;

	XMFLOAT3
		m_ColourDiffuse,
		m_ColourTransparent;

	bool
		m_bWireframe,
		m_bTwoSided;

	CComPtr<ID3D11RasterizerState> m_pRasterState;

	void MakeCurrent(CComPtr<ID3D11DeviceContext> pContext);
};