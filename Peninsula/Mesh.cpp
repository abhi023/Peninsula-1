#include "pch.h"
#include "Mesh.h"

CMesh::CMesh()
{
	OutputDebugString(L"CMesh created\n");
	mTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

CMesh::~CMesh()
{
	OutputDebugString(L"CMesh destroyed\n");
}