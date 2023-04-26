#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "vertex_buffer.h"
#include "index_buffer.h"

#include <vector>

struct VertexArray {
	void create();
	void destroy();

	void addVertexBuffer(VertexBuffer vb);
	void setIndexBuffer(IndexBuffer ib);

	void bind();
	void unbind();

	inline unsigned int getId() const { return id; }
private:
	uint id;
	
	std::vector<VertexBuffer> vbs;
	IndexBuffer ib;
};

#endif
