/*
 * main.cpp
 *
 *  Created on: 24/09/2014
 *      Author: Elliot Wilde
 */

#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include <cstdio>
#include <iostream>
#include "loadShader.h"
#include "loadAssets.hpp"
#include <vector>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

using namespace glm;

GLint width=640, height=480;
GLFWwindow* window;
void display();
void loadAssets();
double lastframe;

GLuint phongshader;
GLuint camerashader;

uint vaoID[1];
uint vboID[3];
int vcount, tcount, ncount;

vec4 light(.5, 10, 2, 1.0);

int main(void){

	if(!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(width, height, "Comp308 Final Project", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		std::printf("windows not built, opengl version too high?");
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental=true;
	if(glewInit() != GLEW_OK){
		printf("glew init error\n");
		return -1;
	}

	loadAssets();

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	phongshader = LoadShaders("shaders/vert.glsl", "shaders/phong_frag.glsl");


	while(!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void loadAssets(){
	std::vector<vec3> v;
	std::vector<vec3> n;
	std::vector<vec2> t;
	readOBJ("assets/thing.obj", v,n, t);
	std::cout<<v.size() << " " << n.size() << " " << t.size() << endl;
	vcount = v.size();
	ncount = n.size();
	tcount = t.size();

	glGenVertexArrays(1, &vaoID[0]);
	glBindVertexArray(vaoID[0]);

	glGenBuffers(3, vboID);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, vcount*sizeof(vec3), &v[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	if (ncount>0){
		glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
		glBufferData(GL_ARRAY_BUFFER, ncount*sizeof(vec3), &n[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
	if(tcount>0){
		glBindBuffer(GL_ARRAY_BUFFER, vboID[2]);
		glBufferData(GL_ARRAY_BUFFER, tcount*sizeof(vec2), &t[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
	}
	glBindVertexArray(0);

}

float rot = 0;

void display(){
	double framestart = glfwGetTime();
		double frametime = framestart-lastframe;
		char cbuf[10];
			sprintf(cbuf, "%.1f", 1./frametime);
			glfwSetWindowTitle(window,cbuf);
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
	glUseProgram(phongshader);

	mat4 ProjectionMatrix = perspective(45.0f, float(width) / float(height), 0.1f, 100.0f);;
	mat4 ViewMatrix = lookAt(
			vec3(15,13,13),
			vec3(0,0,0),
			vec3(0,1,0));
	mat4 ModelMatrix = mat4(1.0);
	ModelMatrix = rotate(ModelMatrix, rot+=frametime, vec3(0,1,0));
	mat4 normalMatrix = transpose(inverse(ViewMatrix * ModelMatrix));

	GLuint ModelID = glGetUniformLocation(phongshader, "M");
	GLuint ViewID = glGetUniformLocation(phongshader, "V");
	GLuint ProjID = glGetUniformLocation(phongshader, "P");
	GLuint NormID = glGetUniformLocation(phongshader, "normalMatrix");
	GLuint lightID = glGetUniformLocation(phongshader, "light");
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewID, 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix4fv(ProjID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(NormID, 1, GL_FALSE, &normalMatrix[0][0]);
	glUniform4fv(lightID,1 , &light[0]);




	glBindVertexArray(vaoID[0]);
	glDrawArrays(GL_TRIANGLES, 0, vcount);
	glBindVertexArray(0);
//	glDrawArrays



	lastframe = framestart;
}
