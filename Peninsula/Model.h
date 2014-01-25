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

	void ProcessMaterials(const aiScene* scene, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);
	void ProcessMeshes(const aiScene* scene, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);
	void ProcessAnimations(const aiScene* scene, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);

	void CalculateNormals(Vertex* pVertices, vector<Vertex>::size_type size, const vector<unsigned int> &indices);

public:
	CModel();
	~CModel();

	virtual void LoadFromFile(const wstring& filename, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext);

	virtual void Render(CComPtr<ID3D11DeviceContext> context);
};

