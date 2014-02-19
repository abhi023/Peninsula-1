#include "pch.h"

#include "Graphics.h"
#include "peninsula.h"
#include "Material.h"
#include "Mesh.h"

#include "Model.h"

const wstring modeldirectory(L"Assets/Models/");
const wstring environmentdirectory(L"Assets/Environment/");
const aiVector3D up_x(1.f, 0.f, 0.f);
const aiVector3D up_y(0.f, 1.f, 0.f);
const aiVector3D up_z(0.f, 0.f, 1.f);

const map<aiReturn, string> assimperrortext =
{
	{ aiReturn_SUCCESS, "No error (0)" },
	{ aiReturn_FAILURE, "The operation failed (-1)" },
	{ aiReturn_OUTOFMEMORY, "Not enough memory was available to perform the operation (-3)" }
};

CModel::CModel() : m_pVertexShader(nullptr), m_pPixelShader(nullptr), m_pGeometryShader(nullptr)
{
}

CModel::~CModel()
{
}

void CModel::ProcessMaterials(const aiScene* pScene, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext)
{
	aiReturn r;
	vector<aiMaterial*> ai_materials(pScene->mMaterials, pScene->mMaterials + pScene->mNumMaterials);

	for (const auto& mat : ai_materials)
	{
		CMaterial model_material;

		aiString name;
		r = mat->Get(AI_MATKEY_NAME, name);
		if (r != aiReturn_SUCCESS)
		{
			throw exception(string("Failed to read name of material. Assimp error code: " + assimperrortext.at(r)).c_str());
		}

		//---------------------- DIFFUSE
		aiColor3D colour(0.f, 0.f, 0.f);
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, colour);
		model_material.m_ColourDiffuse = XMFLOAT3(colour.r, colour.b, colour.g);

		unsigned int diffuse_tex_count = mat->GetTextureCount(aiTextureType_DIFFUSE);

		if (diffuse_tex_count == 1)
		{
			aiString path;
			r = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);

			string nfn(path.C_Str());
			wstring imagefilename(nfn.begin(), nfn.end());
			imagefilename.insert(0, modeldirectory);

			HRESULT hr = CreateWICTextureFromFile(pDevice, pContext, imagefilename.c_str(), nullptr, &model_material.m_pTextureDiffuse.p);
			if (failed(hr))
				throw error(__FILE__, __LINE__, hr);
		}
		else if (diffuse_tex_count > 1)
		{
			throw exception("File contains too many textures of type aiTextureType_DIFFUSE");
		}

		//---------------------- TRANSPARENT COLOUR
		mat->Get(AI_MATKEY_COLOR_TRANSPARENT, colour);
		model_material.m_ColourTransparent = XMFLOAT3(colour.r, colour.g, colour.b);

		//---------------------- NORMAL MAP
		unsigned int normal_map_count = mat->GetTextureCount(aiTextureType_NORMALS);

		if (normal_map_count == 1)
		{
			aiString path;
			r = mat->GetTexture(aiTextureType_NORMALS, 0, &path);

			string nfn(path.C_Str());
			wstring imagefilename(nfn.begin(), nfn.end());
			imagefilename.insert(0, modeldirectory);

			HRESULT hr = CreateWICTextureFromFile(pDevice, pContext, imagefilename.c_str(), nullptr, &model_material.m_pTextureNormals.p);
			if (failed(hr))
				throw error(__FILE__, __LINE__, hr);
		}
		else if (normal_map_count > 1)
		{
			throw exception("File contains too many textures of type aiTextureType_NORMAL");
		}
		
		//---------------------- OPACITY MAP
		unsigned int opacity_map_count = mat->GetTextureCount(aiTextureType_OPACITY);

		if (normal_map_count == 1)
		{
			aiString path;
			r = mat->GetTexture(aiTextureType_OPACITY, 0, &path);

			string nfn(path.C_Str());
			wstring imagefilename(nfn.begin(), nfn.end());
			imagefilename.insert(0, modeldirectory);

			HRESULT hr = CreateWICTextureFromFile(pDevice, pContext, imagefilename.c_str(), nullptr, &model_material.m_pTextureOpacity.p);
			if (failed(hr))
				throw error(__FILE__, __LINE__, hr);
		}
		else if (normal_map_count > 1)
		{
			throw exception("File contains too many textures of type aiTextureType_OPACITY");
		}

		//---------------------- MISC PROPERTIES
		int iTwoSides = 0;
		mat->Get(AI_MATKEY_TWOSIDED, iTwoSides);
		model_material.m_bTwoSided = iTwoSides ? true : false;

		int iWireframe = 0;
		mat->Get(AI_MATKEY_ENABLE_WIREFRAME, iWireframe);
		model_material.m_bWireframe = iWireframe ? true : false;

		model_material.m_bTwoSided = true;

		//---------------------- RASTERIZER STATE
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd);
		rd.CullMode = model_material.m_bTwoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;
		rd.FillMode = model_material.m_bWireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
		rd.DepthClipEnable = TRUE;

		HRESULT hr = pDevice->CreateRasterizerState(&rd, &model_material.m_pRasterState.p);
		if (failed(hr))
		{
			throw error(__FILE__, __LINE__, hr);
		}

		//---------------------- AND FINALLY...
		materials.push_back(make_shared<CMaterial>(model_material));
	}
}
void CModel::ProcessMeshes(const aiScene* pScene, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext)
{
	HRESULT hr = S_OK;

	for (unsigned int mesh_itr = 0; mesh_itr < pScene->mNumMeshes; mesh_itr++)
	{
		aiMesh* ai_CurrentMesh = pScene->mMeshes[mesh_itr];
		CMesh model_CurrentMesh;

		vector<Vertex> aVerts;
		vector<unsigned int> aIndices;

		if (ai_CurrentMesh->HasPositions() == false)
		{
			throw exception("Model does not have vertex data!");
		}

		if (ai_CurrentMesh->HasNormals() == false)
		{
			throw exception("Model does not have normal data!");
		}

		if (ai_CurrentMesh->HasFaces() == false)
		{
			throw exception("Model does not contain face data!");
		}

		switch (ai_CurrentMesh->mPrimitiveTypes)
		{
		case aiPrimitiveType_POINT:
			model_CurrentMesh.mTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;

		case aiPrimitiveType_LINE:
			model_CurrentMesh.mTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			break;

		case aiPrimitiveType_TRIANGLE:
			model_CurrentMesh.mTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;

		default:
			throw exception("Invalid primitive type!");
		}

		ofstream ofile("vertices.txt");

		float min[3] = { 0, 0, 0 }, max[3] = { 0, 0, 0 };

		// load vertices
		for (unsigned int vertex_itr = 0; vertex_itr < ai_CurrentMesh->mNumVertices; vertex_itr++)
		{
			Vertex temp;
			temp.position.x = ai_CurrentMesh->mVertices[vertex_itr].x;
			temp.position.y = ai_CurrentMesh->mVertices[vertex_itr].y;
			temp.position.z = ai_CurrentMesh->mVertices[vertex_itr].z;

			if (ai_CurrentMesh->HasNormals())
			{
				temp.normal.x = ai_CurrentMesh->mNormals[vertex_itr].x;
				temp.normal.y = ai_CurrentMesh->mNormals[vertex_itr].y;
				temp.normal.z = ai_CurrentMesh->mNormals[vertex_itr].z;
			}

			temp.texture.x = ai_CurrentMesh->mTextureCoords[0][vertex_itr].x;
			temp.texture.y = ai_CurrentMesh->mTextureCoords[0][vertex_itr].y;

			ofile << "Vertex number: " << vertex_itr << endl
				<< "- pos: x = " << temp.position.x << ", y = " << temp.position.y << ", z = " << temp.position.z << endl
				<< "- normal: x = " << temp.normal.x << ", y = " << temp.normal.y << ", z = " << temp.normal.z << endl
				<< "- texture: u = " << temp.texture.x << ", v = " << temp.texture.y << endl
				<< endl;
			
			swap(temp.position.y, temp.position.z);
			temp.position.y *= -1;

			swap(temp.normal.y, temp.normal.z);
			temp.normal.y *= -1;

			aVerts.push_back(temp);

			min[0] = minimum(temp.position.x, min[0]);
			min[1] = minimum(temp.position.y, min[1]);
			min[2] = minimum(temp.position.z, min[2]);

			max[0] = maximum(temp.position.x, max[0]);
			max[1] = maximum(temp.position.y, max[1]);
			max[2] = maximum(temp.position.z, max[2]);
		}

		// load indices
		for (uint32_t face_itr = 0; face_itr < ai_CurrentMesh->mNumFaces; face_itr++)
		{
			ofile << "Triangle number: " << face_itr << endl;
			for (uint32_t index_itr = 0; index_itr < ai_CurrentMesh->mFaces[face_itr].mNumIndices; index_itr++)
			{
				if (ai_CurrentMesh->mFaces[face_itr].mIndices[index_itr] >= aVerts.size())
				{
					throw exception("Invalid index specification!");
				}
				
				ofile << "- " << face_itr << " = " << ai_CurrentMesh->mFaces[face_itr].mIndices[index_itr] << endl;

				aIndices.push_back(ai_CurrentMesh->mFaces[face_itr].mIndices[index_itr]);
			}

			ofile << endl;
		}

		if (ai_CurrentMesh->HasNormals() == false)
		{
			CalculateNormals(&aVerts[0], aVerts.size(), aIndices);
		}

		// Sanity checks
		if (aVerts.size() == 0)
		{
			throw exception("No vertices loaded!");
		}

		if (aIndices.size() == 0)
		{
			throw exception("No indices loaded!");
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.ByteWidth = aVerts.size() * sizeof(aVerts[0]);
		bd.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd);
		sd.pSysMem = &aVerts[0];

		hr = pDevice->CreateBuffer(&bd, &sd, &model_CurrentMesh.m_pVertexBuffer.p);
		if (failed(hr))
		{
			throw error(__FILE__, __LINE__, hr);
		}

		// Create index buffer
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.ByteWidth = aIndices.size() * sizeof(aIndices[0]);
		bd.Usage = D3D11_USAGE_DEFAULT;
		sd.pSysMem = &aIndices[0];

		hr = pDevice->CreateBuffer(&bd, &sd, &model_CurrentMesh.m_pIndexBuffer.p);
		if (failed(hr))
		{
			throw error(__FILE__, __LINE__, hr);
		}

		model_CurrentMesh.m_uIndices = aIndices.size();
		model_CurrentMesh.m_uMaterialIndex = ai_CurrentMesh->mMaterialIndex;

		meshes.push_back(make_shared<CMesh>(model_CurrentMesh));
	}
}

void CModel::LoadFromFile(const wstring& filename, CComPtr<ID3D11Device> pDevice, CComPtr<ID3D11DeviceContext> pContext)
{
	OutputDebugString(wstring(L"Loading file " + filename + L"\n").c_str());

	HRESULT hr = S_OK;

	Assimp::Importer importer;

	string narrowfilename(filename.begin(), filename.end());

	const unsigned int processflags =
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ValidateDataStructure |
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FindDegenerates |
		aiProcess_SortByPType;
	
	const aiScene* scene = importer.ReadFile(narrowfilename.c_str(), processflags);

	if (scene == nullptr)
	{
		throw exception(importer.GetErrorString());
	}

	if (scene->HasMaterials())
	{
		OutputDebugStringW(wstring(L"Processing materials for " + filename + L"\n").c_str());
		ProcessMaterials(scene, pDevice, pContext);
	}
	else
	{
		OutputDebugStringW(L"No materials found in model...\n");
	}
	
	aiVector3D up(0.f, 1.f, 0.f);
	if (scene->HasCameras())
	{
		up = scene->mCameras[0]->mUp;
	}
	else
	{
		OutputDebugStringW(L"No cameras found for model. Assuming model is y-up...\n");
	}

	if (scene->HasMeshes())
	{
		OutputDebugStringW(wstring(L"Processing meshes for " + filename + L"\n").c_str());
		
		ProcessMeshes(scene, pDevice, pContext);
	}
	else
	{
		OutputDebugStringW(L"No meshes found in model...\n");
	}
}

void CModel::CalculateNormals(Vertex* vecVerts, vector<Vertex>::size_type size, const vector<unsigned int> &indices)
{
	for (UINT vert_itr = 0; vert_itr < size; vert_itr += 3)
	{
		XMVECTOR t1 = XMLoadFloat3(&(vecVerts)[indices[vert_itr]].position);
		XMVECTOR t2 = XMLoadFloat3(&(vecVerts)[indices[vert_itr + 1]].position);
		XMVECTOR t3 = XMLoadFloat3(&(vecVerts)[indices[vert_itr + 2]].position);

		XMVECTOR v1 = XMVectorSubtract(t2, t1);

		XMVECTOR v2 = XMVectorSubtract(t3, t1);

		XMVECTOR vNormal = XMVector3Cross(v1, v2);

		XMVECTOR vLHAdjust = XMVectorSet(1, -1, 1, 0);
		vNormal = XMVectorMultiply(vNormal, vLHAdjust);

		vNormal = XMVector3Normalize(vNormal);

		for (UINT corner_itr = 0; corner_itr < 3; corner_itr++)
		{
			XMVECTOR vVertexNormal = XMLoadFloat3(&(vecVerts)[indices[vert_itr + corner_itr]].normal);

			vVertexNormal = XMVectorAdd(vNormal, vVertexNormal);

			vVertexNormal = XMVector3Normalize(vVertexNormal);

			XMStoreFloat3(&(vecVerts)[indices[vert_itr + corner_itr]].normal, vVertexNormal);
		}
	}
}

void CModel::Render(CComPtr<ID3D11DeviceContext> context)
{
	CComPtr<ID3D11VertexShader> pVSDefault = nullptr;
	CComPtr<ID3D11PixelShader> pPSDefault = nullptr;
	CComPtr<ID3D11GeometryShader> pGSDefault = nullptr;

	if (m_pVertexShader != nullptr)
	{
		context->VSGetShader(&pVSDefault.p, nullptr, 0);
		context->VSSetShader(m_pVertexShader, nullptr, 0);
	}
	if (m_pPixelShader != nullptr)
	{
		context->PSGetShader(&pPSDefault.p, nullptr, 0);
		context->PSSetShader(m_pPixelShader, nullptr, 0);
	}
	if (m_pGeometryShader != nullptr)
	{
		context->GSGetShader(&pGSDefault.p, nullptr, 0);
		context->GSSetShader(m_pGeometryShader, nullptr, 0);
	}

	for (const auto& mesh_itr : meshes)
	{
		// sanity checks
		if (mesh_itr->m_pVertexBuffer == nullptr) continue;
		if (mesh_itr->m_pIndexBuffer == nullptr) continue;
		if (mesh_itr->m_uIndices == 0) continue;

		// Select material
		materials[mesh_itr->m_uMaterialIndex]->MakeCurrent(context);

		// Select vertex buffer
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &mesh_itr->m_pVertexBuffer.p, &stride, &offset);

		// Select index buffer
		context->IASetIndexBuffer(mesh_itr->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//context->IASetPrimitiveTopology(parts_it.mTopology);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(mesh_itr->m_uIndices, 0, 0);
	}

	// if we changed the shader away from the default, set it back
	if (pVSDefault != nullptr)
	{
		context->VSSetShader(pVSDefault, nullptr, 0);
	}
	if (pPSDefault != nullptr)
	{
		context->PSSetShader(pPSDefault, nullptr, 0);
	}
	if (pGSDefault != nullptr)
	{
		context->GSSetShader(pGSDefault, nullptr, 0);
	}
}