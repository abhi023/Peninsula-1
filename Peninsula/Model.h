#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Material.h"

struct Vertex;

class CModel
{
protected:
	vector<shared_ptr<CMesh>> meshes;
	vector<shared_ptr<CMaterial>> materials;

	CComPtr<ID3D11VertexShader> m_pVertexShader;
	CComPtr<ID3D11PixelShader> m_pPixelShader;
	CComPtr<ID3D11GeometryShader> m_pGeometryShader;
	CComPtr<ID3D11InputLayout> m_pInputLayout;

	void ProcessMaterials(const aiScene* scene, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);
	void ProcessMeshes(const aiScene* scene, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);
	void ProcessAnimations(const aiScene* scene, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);

	void CalculateNormals(Vertex* pVertices, vector<Vertex>::size_type size, const vector<unsigned int> &indices);

public:
	CModel();
	~CModel();

	virtual void LoadFromFile(const wstring& filename, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);

	void SetVS(CComPtr<ID3D11VertexShader> vs)
	{
		m_pVertexShader = vs;
	}
	void SetPS(CComPtr<ID3D11PixelShader> ps)
	{
		m_pPixelShader = ps;
	}
	void SetGS(CComPtr<ID3D11GeometryShader> gs)
	{
		m_pGeometryShader = gs;
	}
	void SetIL(CComPtr<ID3D11InputLayout> il)
	{
		m_pInputLayout = il;
	}

	virtual void Render(CComPtr<ID3D11DeviceContext> context);
};

