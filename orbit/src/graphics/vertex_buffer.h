#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "vertex_attribute.h"

#include <vector>

struct VertexBuffer {
	void create(void* verts, unsigned int size, std::vector<VertexAttribute> attribs);
	void destroy();
	
	void bind();
	void unbind();
	
	inline const std::vector<VertexAttribute>& getAttributeList() { return attrib_list; }
	inline unsigned int getId() const { return id; }
	
private:
	std::vector<VertexAttribute> attrib_list;
	unsigned int id;
};

#endif // VERTEX_BUFFER_H
