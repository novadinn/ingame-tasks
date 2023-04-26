#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct IndexBuffer {
	void create(unsigned int* inds, unsigned int size);
	void destroy();
	
	void bind();
	void unbind();

	inline unsigned int getIndicesCount() const { return count; }
	
	inline unsigned int getId() const { return id; }
	
private:
	unsigned int id;
	unsigned int count;
};

#endif // INDEX_BUFFER_H
