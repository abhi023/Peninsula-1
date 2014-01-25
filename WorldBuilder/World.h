#pragma once

#include "pch.h"

const size_t world_width = 256;
const size_t world_height = 256;

enum skybox_parts {
	ground,
	sky,
	north,
	south,
	east,
	west
};

class World
{
protected:
	CComPtr<ID3D11ShaderResourceView> m_pTexHeightmap;
	
	array<array<float, world_height>, world_width> m_aHeightmap;

	CComPtr<ID3D11ShaderResourceView> m_pTexWorld; // skybox and ground

	CComPtr<ID3D11Buffer> m_pVertexBuffer, m_pIndexBuffer;

	CComPtr<ID3D11VertexShader> m_pVertexShader;
	CComPtr<ID3D11PixelShader> m_pPixelShader;
	CComPtr<ID3D11GeometryShader> m_pGeometryShader;
	
public:
	World();
	~World();

	void Initialise(CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);
};

