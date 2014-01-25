#pragma once
#include "pch.h"
#include "Model.h"

class Marker : public CModel
{
protected:
	CComPtr<ID3D11Buffer> m_pVertexBuffer;

	CComPtr<ID3D11VertexShader> m_pVertexShader;
	CComPtr<ID3D11PixelShader> m_pPixelShader;
	CComPtr<ID3D11InputLayout> m_pInputLayout;

public:	
	HRESULT Load(CComPtr<ID3D11Device> pDevice, 
		CComPtr<ID3D11DeviceContext> pContext);

	void Render(CComPtr<ID3D11DeviceContext> context);
};