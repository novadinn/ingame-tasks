#ifndef GLM_EXTENSION_H
#define GLM_EXTENSION_H

#include <glm/glm.hpp>

glm::vec3 polar_to_cartesian(float r, float alpha, float polar) {
	return glm::vec3(r * cos(alpha) * cos(polar),
					 r * sin(alpha),
					 r * cos(alpha) * sin(polar));
}

#endif // GLM_EXTENSION_H
