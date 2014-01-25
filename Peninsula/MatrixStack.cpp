#include "pch.h"
#include "MatrixStack.h"
#include "Graphics.h"

MatrixStack::MatrixStack()
{
	number_on_stack = 0;
}

MatrixStack::~MatrixStack()
{
}

void MatrixStack::push(const XMMATRIX& mat)
{
	if (number_on_stack >= 16)
		throw exception(__FILE__, __LINE__);

	stack[number_on_stack] = mat;

	number_on_stack++;

	updatebuffers();
}

void MatrixStack::push_last(const XMMATRIX& mat)
{
	lastmat = mat;

	updatebuffers();
}

void MatrixStack::pop()
{
	if (number_on_stack == 0)
		throw exception("Nothing on stack to pop!");

	number_on_stack--;

	updatebuffers();
}

void MatrixStack::flush()
{
	number_on_stack = 0;

	lastmat = XMMatrixIdentity();

	updatebuffers();
}

XMMATRIX MatrixStack::evaluate()
{
	XMMATRIX rmat = XMMatrixIdentity();

	for (XMMATRIX* matrix_itr = &stack[0]; matrix_itr != &stack[0] + number_on_stack; matrix_itr++)
	{
		rmat = XMMatrixMultiply(rmat, *matrix_itr);
	}

	rmat = XMMatrixMultiply(rmat, lastmat);

	return rmat;
}

void MatrixStack::updatebuffers()
{
	CBWorld cb;
	cb.mWorld = XMMatrixTranspose(evaluate());

	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms);

	HRESULT hr = m_pContext->Map(m_pCBWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	if (failed(hr))
		throw error(__FILE__, __LINE__, hr);

	memcpy_s(ms.pData, sizeof(cb), &cb, sizeof(cb));
	m_pContext->Unmap(m_pCBWorld, 0);
}