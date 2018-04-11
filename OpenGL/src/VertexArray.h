#pragma once

#include "VertexBuffer.h"

//Tivemos que remover este include, pois ele tambem inclui o Renderer.h o que causa
//looping infinito no pipeline. Para resolver, declaramos aqui a classe que precisamos
// deste header file que e o VertexBufferLayout e nao o documento inteiro com os includes.

//#include "VertexBufferLayout.h"

class VertexBufferLayout; // Nao precisamos do header.

class VertexArray
{
private:
	unsigned int m_RendererID;

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;
};

