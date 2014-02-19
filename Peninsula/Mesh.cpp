#include "pch.h"
#include "Mesh.h"

CMesh::CMesh() : m_pVertexBuffer(nullptr), m_pIndexBuffer(nullptr), m_pBoundingVertex(nullptr), m_pBoundingIndex(nullptr), mTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST), m_strMeshName(), m_uIndices(0), m_uMaterialIndex(0)
{
}

CMesh::~CMesh()
{
}