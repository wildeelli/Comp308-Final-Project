/*
 * Mesh.hpp
 *
 *  Created on: 9/10/2014
 *      Author: Elliot Wilde
 */

#ifndef MESH_HPP_
#define MESH_HPP_
#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <vector>
#include "Triangle.hpp"

//using namespace glm;
//using namespace std;

class Mesh {
public:
	Mesh();
private:
	std::vector<Triangle> faces;
	GLuint vao;
	GLuint vbo[3];
	std::vector<glm::vec3> colours;
};

#endif /* MESH_HPP_ */
