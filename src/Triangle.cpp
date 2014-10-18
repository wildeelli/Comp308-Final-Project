/*
 * Triangle.cpp
 *
 *  Created on: 9/10/2014
 *      Author: Elliot Wilde
 */
#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "Triangle.hpp"
#include <cstdio>

Triangle::Triangle(glm::vec3 v, glm::vec3 n, glm::vec4 c) {
	vertex=v;
	normal=n;
	colour=c;
	pickcolour=glm::vec3(v[0], v[1], v[2]);
//	printf("Triangle\n");
	constructVBO();
}

glm::vec3 Triangle::getPickColour(){
	return glm::vec3(pickcolour);
}

void Triangle::draw(){
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0,1);
	glBindVertexArray(0);
}

void Triangle::constructVBO(){
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &vertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

//	if (&normal!=NULL){
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &normal[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
//	}
//	if (&colour!=NULL){
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4), &colour[0], GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)2, 4, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(2);
//	}
//	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &colour[0], GL_STATIC_DRAW);
//	glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(3);
		glBindVertexArray(0);
}
