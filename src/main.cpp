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
GLuint shadowmapshader;

uint vaoID[1];
uint vboID[3];
uint cubevao[1];
uint cubevbo[2];
int vcount[2], tcount[2], ncount[2];

GLint ModelID;
GLint ViewID ;
GLint ProjID ;
GLint NormID ;
GLint lightID;
GLint diffuseColorID ;
GLint specularColorID;

vec4 light(10.5, 10, 2, 1.0);
//vec4 light(-8, 3, 3, 1.0);

int main(void){

	if(!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_SAMPLES, 32);
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

	GLenum err = glGetError();
		if (err!=GL_NO_ERROR){
			printf("error code (a): {%d}\n", err);
		}
	loadAssets();

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	phongshader = LoadShaders("shaders/vert.glsl", "shaders/phong_frag.glsl");
	shadowmapshader = LoadShaders("shaders/svert.glsl", "shaders/sfrag.glsl");

	if ((ModelID = glGetUniformLocation(phongshader, "M"))==-1) printf("Error getting uniform location - Model\n");
	if ((err=glGetError())!=GL_NO_ERROR) printf("error code (M): {%d}\n", err);
	if ((ViewID = glGetUniformLocation(phongshader, "V"))==-1) printf("Error getting uniform location - View\n");
	if ((err=glGetError())!=GL_NO_ERROR) printf("error code (V): {%d}\n", err);
	if ((ProjID = glGetUniformLocation(phongshader, "P"))==-1) printf("Error getting uniform location - Projection\n");
	if ((err=glGetError())!=GL_NO_ERROR) printf("error code (P): {%d}\n", err);
	if ((NormID = glGetUniformLocation(phongshader, "normalMatrix"))==-1) printf("Error getting uniform location - normalMatrix\n");
	if ((err=glGetError())!=GL_NO_ERROR) printf("error code (N): {%d}\n", err);
	if ((lightID = glGetUniformLocation(phongshader, "light"))==-1) printf("Error getting uniform location - Light\n");
	if ((err=glGetError())!=GL_NO_ERROR) printf("error code (L): {%d}\n", err);
	if ((diffuseColorID = glGetUniformLocation(phongshader, "diffuse_colour")  )==-1) printf("Error getting uniform location - diffuse\n");
	if ((err=glGetError())!=GL_NO_ERROR) printf("error code (D): {%d}\n", err);
	if ((specularColorID = glGetUniformLocation(phongshader, "specular_colour"))==-1) printf("Error getting uniform location - specular\n");
	if ((err=glGetError())!=GL_NO_ERROR) printf("error code (S): {%d}\n", err);

	err = glGetError();
	if (err!=GL_NO_ERROR){
		printf("error code (b): {%d}\n", err);
	}

	while(!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	glDeleteVertexArrays(1, &vaoID[0]);
	glDeleteVertexArrays(1, &cubevao[0]);
	return 0;
}

void loadAssets(){
	std::vector<vec3> v;
	std::vector<vec3> n;
	std::vector<vec2> t;
	readOBJ("assets/sphere.obj", v,n, t);
	std::cout<<v.size() << " " << n.size() << " " << t.size() << endl;
	vcount[0] = v.size();
	ncount[0] = n.size();
	tcount[0] = t.size();

	glGenVertexArrays(1, &vaoID[0]);
	glBindVertexArray(vaoID[0]);

	glGenBuffers(3, vboID);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, vcount[0]*sizeof(vec3), &v[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	if (ncount[0]>0){
		glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
		glBufferData(GL_ARRAY_BUFFER, ncount[0]*sizeof(vec3), &n[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
	if (tcount[0]>0){
		glBindBuffer(GL_ARRAY_BUFFER, vboID[2]);
		glBufferData(GL_ARRAY_BUFFER, tcount[0]*sizeof(vec2), &t[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
	}
	glBindVertexArray(0);

	v.clear();
	n.clear();
	t.clear();

	readOBJ("assets/dice.obj", v,n, t);
	std::cout<<v.size() << " " << n.size() << " " << t.size() << endl;
 	vcount[1] = v.size();
	ncount[1] = n.size();
	tcount[1] = t.size();

	glGenVertexArrays(1, &cubevao[0]);
	glBindVertexArray(cubevao[0]);

	glGenBuffers(2, cubevbo);

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vcount[1]*sizeof(vec3), &v[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	if (ncount[1]>0){
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
		glBufferData(GL_ARRAY_BUFFER, ncount[1]*sizeof(vec3), &n[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
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
			vec3(3,4,3),
			vec3(0,0,0),
			vec3(0,1,0));
//	ModelMatrix = scale(ModelMatrix, vec3(2));
//	ViewMatrix = rotate(ViewMatrix, rot*4, normalize(vec3(1,0,1)));

	glUniformMatrix4fv(ViewID, 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix4fv(ProjID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniform4fv(lightID,1 , &light[0]);




	mat4 ModelMatrix = mat4(1.0);
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelMatrix[0][0]);
	mat4 normalMatrix = transpose(inverse(ViewMatrix * ModelMatrix));
	glUniformMatrix4fv(NormID, 1, GL_FALSE, &normalMatrix[0][0]);
	vec3 diffuse = vec3(.8f, .8f, .8f);
	glUniform3fv(diffuseColorID, 1, &diffuse[0]);
	vec4 specular = vec4(.6, 1., .6, 55);
	glUniform4fv(specularColorID, 1, &specular[0]);
	glBindVertexArray(vaoID[0]);
	glDrawArrays(GL_TRIANGLES, 0, vcount[0]);
	glBindVertexArray(0);

	ModelMatrix = mat4(1.0);
	ModelMatrix = rotate(ModelMatrix, rot+=frametime, vec3(1,0,0));
	ModelMatrix = translate(ModelMatrix, vec3(0, 0, 3));
	ModelMatrix = rotate(ModelMatrix, rot*4, vec3(.707, 0 ,.707));
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelMatrix[0][0]);
	normalMatrix = transpose(inverse(ViewMatrix * ModelMatrix));
	glUniformMatrix4fv(NormID, 1, GL_FALSE, &normalMatrix[0][0]);
	diffuse = vec3(1., .2, .2);
	glUniform3fv(diffuseColorID, 1, &diffuse[0]);
	specular = vec4(.08, .08, 0.8, 128);
	glUniform4fv(specularColorID, 1, &specular[0]);
	glBindVertexArray(cubevao[0]);
	glDrawArrays(GL_TRIANGLES, 0, vcount[1]);
	glBindVertexArray(0);



//	glDrawArrays



	lastframe = framestart;
}

void renderFB(){

}
