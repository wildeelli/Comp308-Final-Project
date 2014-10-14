/*
 * Triangle.hpp
 *
 *  Created on: 9/10/2014
 *      Author: Elliot Wilde
 */

#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class Triangle {
public:
	Triangle(glm::vec3 v, glm::vec3 n, glm::vec4 c);
	glm::vec3 getPickColour();
	void draw();

private:
	glm::vec3 vertex;
	glm::vec3 normal;
	glm::vec4 colour;
	glm::vec3 pickcolour;
	GLuint vao;
	GLuint vbo[5];

	void constructVBO();
};

#endif /* TRIANGLE_HPP_ */
