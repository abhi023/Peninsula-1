#pragma once

class CGraphics;

class MatrixStack
{
protected:
	friend CGraphics;

	CComPtr<ID3D11DeviceContext> m_pContext;
	CComPtr<ID3D11Buffer> m_pCBWorld;

	MatrixStack();
	~MatrixStack();

	__declspec(align(16)) XMMATRIX stack[16];
	__declspec(align(16)) XMMATRIX lastmat;

	std::size_t number_on_stack;
	std::size_t size;

	void push(const XMMATRIX& mat);
	void push_last(const XMMATRIX& mat);
	void pop();
	void flush();
	void resize(std::size_t new_size);

	void updatebuffers();

	XMMATRIX evaluate();
};