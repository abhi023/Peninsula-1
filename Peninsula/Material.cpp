#include "pch.h"
#include "Material.h"
#include "peninsula.h"

//--------------------------------------- CMATERIAL CLASS
CComPtr<ID3D11Buffer> CMaterial::pCBMaterial;

CMaterial::CMaterial()
{

}

CMaterial::~CMaterial()
{

}

void CMaterial::MakeCurrent(CComPtr<ID3D11DeviceContext> pContext)
{
	pContext->RSSetState(m_pRasterState);

	pContext->PSSetShaderResources(0, 1, &m_pTextureDiffuse.p);

	pContext->PSSetShaderResources(1, 1, &m_pTextureNormals.p);

	pContext->PSSetShaderResources(2, 1, &m_pTextureOpacity.p);

	D3D11_MAPPED_SUBRESOURCE map_material;
	ZeroMemory(&map_material);

	HRESULT hr = pContext->Map(pCBMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &map_material);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	CBMaterial matCB =
	{
		XMFLOAT4(m_ColourDiffuse.x, m_ColourDiffuse.y, m_ColourDiffuse.z, 1.f),
		XMFLOAT4(m_ColourTransparent.x, m_ColourTransparent.y, m_ColourTransparent.z, 1.f),
	};

	memcpy(map_material.pData, &matCB, sizeof(matCB));

	pContext->Unmap(pCBMaterial, 0);
}