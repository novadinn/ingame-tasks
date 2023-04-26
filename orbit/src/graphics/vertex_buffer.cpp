#include "vertex_buffer.h"

void VertexBuffer::create(void* verts, uint size, std::vector<VertexAttribute> attribs) {
	attrib_list = attribs;
	glCreateBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, size, verts, GL_STATIC_DRAW);
}
    
void VertexBuffer::destroy() {
	glDeleteBuffers(1, &id);
}
    
void VertexBuffer::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, id);
}
    
void VertexBuffer::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
