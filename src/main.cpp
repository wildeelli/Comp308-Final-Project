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
#include "Triangle.hpp"
#include <cstdlib>

#include "HeightField.h"

#include <cstring>
#include <cstdio>
#include <png.h>
//Magic defines for libPNG because they removed them from the library
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#define png_voidp_NULL NULL


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

using namespace glm;

GLint width=800, height=600;
GLint dwidth=640, dheight=480;
GLFWwindow* window;
void display();
void loadAssets();
bool createFrameBuffer();
GLuint createCubeMapTexture();
void createFluidVBO();
void updateFluidVBO();
void renderCubeMap(vec3);
void keyboardListener(GLFWwindow*, int, int, int, int);

double lastframe;

GLuint phongshader;
GLuint camerashader;
GLuint shadowmapshader;
GLuint isvisibleshader;
GLuint cubemapshader;

GLuint detectionBuffer;
GLuint detectionTexture;
GLuint detectionDepthBuffer;

GLuint cubeTexture;
GLuint cubeTextureArray;
GLint cubesize = 1600;

GLuint skyboxTexture;
GLuint skyboxBuffer;


uint vaoID[1];
uint vboID[3];
uint cubevao[1];
uint cubevbo[3];
int vcount[2], tcount[2], ncount[2];

uint fluidvao[1];
uint fluidvbo[2];
uint fluidibo[1];
uint fluidTrisCount, fluidPointsCount;
G308_Triangle* fluidTris;
G308_Point* fluidPoints;

GLint ModelID;
GLint ViewID ;
GLint ProjID ;
GLint NormID ;
GLint lightID;
GLint diffuseColorID ;
GLint specularColorID;
GLint cameraPosID;

GLfloat caretx = 5;
GLfloat carety = 5;
GLuint caretVao;
GLint caretverts;

float cameraRoty = 0;
float cameraRotz = 0;

bool go = false;
bool lines = false;

HeightField* sim;
int gridsize = 64;

std::vector<Triangle> tri;

vec4 light(10.5, 10, 2, 1.0);
//vec4 light(-8, 3, 3, 1.0);

int loadSkyboxTexture(){
	GLuint skybox;
	glGenTextures(1, &skybox);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);

	for (int i=0; i<6; ++i){
		png_structp png_ptr;
		png_infop info_ptr;
		uint sig_read =0;
		FILE *fp;
		const char filename[] = {'a', 's', 's', 'e', 't', 's', '/', char('a'+i), '.', 'p', 'n', 'g', '\0'};
//		std::cout << char('a'+i) << std::endl;
		if ((fp = fopen(filename, "rb")) == NULL){
			std::cout << "Error opening " << filename << std::endl;
			return(0);
		}

		 //Magic, do not touch.
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if (png_ptr == NULL) {
			printf("LibPNG did something terrible!\n");
			fclose(fp);
			exit(1);
		}
		//Allocate the struct we're going to read the data into.
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			printf("LibPNG did something terrible!\n");
			fclose(fp);
			png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
			exit(1);
		}
		//Error handling magic. Do not touch.
		if (setjmp(png_jmpbuf(png_ptr))) {
			printf("LibPNG did something terrible!\n");
			png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
			fclose(fp);
			exit(1);
		}
		//Point libPNG to the right place
		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, sig_read);
		//Yet more magic you shouldn't touch. We -probably- have enough memory to read it all in one go.
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, png_voidp_NULL);
		int width = png_get_image_width(png_ptr,info_ptr);
		int height = png_get_image_height(png_ptr,info_ptr);
		unsigned char* textureData;
		 //How long is a row?
		unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		//Allocate some space!
		textureData = (unsigned char*) malloc(row_bytes * height);

		//Read the image data into the texture array, in reverse row order. Because GL.
		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
		for (int j = 0; j < height; ++j) {
			memcpy(textureData+(row_bytes * (height-1-j)), row_pointers[j], row_bytes);
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+ i, 0,
				(png_get_color_type(png_ptr,info_ptr)==PNG_COLOR_TYPE_RGBA? GL_RGBA:GL_RGB),
				width, height, 0,
				(png_get_color_type(png_ptr,info_ptr)==PNG_COLOR_TYPE_RGBA? GL_RGBA:GL_RGB),
				GL_UNSIGNED_BYTE, textureData );
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(fp);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return skybox;
}

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
		printf("windows not built, opengl version too high?");
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyboardListener);
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

	if (!createFrameBuffer()){
		std::cout << "creating framebuffer failed" << std::endl;
	}
	cubeTexture = createCubeMapTexture();

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	phongshader = LoadShaders("shaders/vert.glsl", "shaders/phong_frag.glsl");
	shadowmapshader = LoadShaders("shaders/svert.glsl", "shaders/sfrag.glsl");
//	isvisibleshader = LoadShaders("shaders/colourpick_vert.glsl", "shaders/colourpick_frag.glsl");
	cubemapshader = LoadShaders("shaders/cubemap_vertex.glsl", "shaders/cubemap_frag.glsl");

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
	if ((cameraPosID = glGetUniformLocation(phongshader, "camerapos"))==-1) printf("Error getting uniform location - camera\n");
	if ((err=glGetError())!=GL_NO_ERROR) printf("error code (C): {%d}\n", err);

	err = glGetError();
	if (err!=GL_NO_ERROR){
		printf("error code (b): {%d}\n", err);
	}

	// dump all the setup info to the console in case it crashes before stdout flushes, and so that eclipse actually shows me stuff
	fflush(stdout);

	sim = new HeightField(gridsize,0.99,-2);
//	sim->push(5,5,1,2);
	sim->update();
	createFluidVBO();

	double lastTime = glfwGetTime();
	int nframes=0;

	if (!(skyboxTexture=loadSkyboxTexture())){
		std::cout << "not loaded cube properly" << std::endl;
	}
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	while(!glfwWindowShouldClose(window))
	{
		double ctime = glfwGetTime();
		nframes++;
		if (ctime - lastTime >= .1 ){
			char cbuf[10];
			sprintf(cbuf, "%.5fms/frame %s", 100.0/double(nframes), (go? "going":"not going"));
			glfwSetWindowTitle(window,cbuf);

			nframes=0;
			lastTime += .1;
		}
		if (go){
			sim->update();
			updateFluidVBO();
		}

		glfwGetFramebufferSize(window, &width, &height);
		renderCubeMap(vec3(gridsize/2.f,-5,gridsize/2.f));
		display();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	glDeleteVertexArrays(1, &vaoID[0]);
	glDeleteVertexArrays(1, &cubevao[0]);
	return 0;
}

bool createFrameBuffer(){
	glGenFramebuffers(1, &detectionBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, detectionBuffer);

	glGenTextures(1, &detectionTexture);
	glBindTexture(GL_TEXTURE_2D, detectionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dwidth, dheight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &detectionDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, detectionDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, dwidth, dheight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, detectionDepthBuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, detectionTexture, 0);
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return false;
	return true;
}

GLuint createCubeMapTexture(){
	GLuint cube;
	glGenTextures(1, &cube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube);

//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//	std::vector<GLubyte> testData(cubesize * cubesize * 256, 128);
//	std::vector<GLubyte> xData(cubesize * cubesize * 256, 255);

	for(int loop = 0; loop < 6; ++loop)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + loop, 0, GL_RGB, cubesize, cubesize, 0, GL_RGB, GL_FLOAT, 0);
//		if(loop)
//		{
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + loop, 0, GL_RGBA8,
//					cubesize, cubesize, 0, GL_RGBA, GL_UNSIGNED_BYTE, &testData[0]);
//		}
//		else
//		{
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + loop, 0, GL_RGBA8,
//					cubesize, cubesize, 0, GL_RGBA, GL_UNSIGNED_BYTE, &xData[0]);
//		}
	}

	GLuint cubeBuffer;
	glGenFramebuffers(1, &cubeBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, cubeBuffer);

	GLuint depth;
	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cubesize, cubesize);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cubeBuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, cube, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return cube;
}

void createFluidVBO(){

//	std::cout << "size: " << sizeof(*sim->triangles) << std::endl;

	for (int i=0; i<2*(gridsize-1)*(gridsize-1); ++i){
//		std::cout << " " << sim->points[sim->triangles[i].v1].x << " " << sim->points[sim->triangles[i].v1].y << " " << sim->points[sim->triangles[i].v1].z <<  " ";
//		std::cout << " " << sim->normals[sim->triangles[i].v1].x << " " << sim->normals[sim->triangles[i].v1].y << " " << sim->normals[sim->triangles[i].v1].z << std::endl;
	}

	fflush(stdout);
	fluidTris = sim->triangles;
	fluidPoints = sim->points;
	fluidTrisCount = 2*(gridsize-1)*(gridsize-1)*3*sizeof(uint);
	fluidPointsCount = gridsize*gridsize*3*sizeof(float);

	glGenVertexArrays(1, &fluidvao[0]);
	glBindVertexArray(fluidvao[0]);

	glGenBuffers(2, fluidvbo);
	glBindBuffer(GL_ARRAY_BUFFER, fluidvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, fluidPointsCount, &sim->points[0], GL_STATIC_DRAW );
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, fluidvbo[1]);
	glBufferData(GL_ARRAY_BUFFER, fluidPointsCount, &sim->normals[0], GL_STATIC_DRAW );
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, fluidibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fluidibo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fluidTrisCount, &sim->triangles[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	std::cout << "size: " << sizeof(*fluidTris) << std::endl;

}

void updateFluidVBO(){
	glBindVertexArray(fluidvao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, fluidvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, gridsize*gridsize*3*sizeof(float), &sim->points[0], GL_STATIC_DRAW );
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, fluidvbo[1]);
	glBufferData(GL_ARRAY_BUFFER, gridsize*gridsize*3*sizeof(float), &sim->normals[0], GL_STATIC_DRAW );
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void loadAssets(){
	std::vector<vec3> v;
	std::vector<vec3> n;
	std::vector<vec2> t;
	readOBJ("assets/caret.obj", v,n, t);
	std::cout<<v.size() << " " << n.size() << " " << t.size() << std::endl;
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
	std::vector<vec3> c;

	readOBJ("assets/dice.obj", v, n, t, c);
	std::cout<<v.size() << " " << n.size() << " " << t.size() << std::endl;
 	vcount[1] = v.size();
	ncount[1] = n.size();
	tcount[1] = t.size();
	std::cout<< c.size() << std::endl;

//	for (int i=0; i<vcount[1]; ++i){
//		tri.push_back(Triangle(v[i], n[i], vec4(1.0)));
//	}

	glGenVertexArrays(1, &cubevao[0]);
	glBindVertexArray(cubevao[0]);

	glGenBuffers(3, cubevbo);

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
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[2]);
	glBufferData(GL_ARRAY_BUFFER, c.size()*sizeof(vec3), &c[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);
	glBindVertexArray(0);

	v.clear();
	n.clear();
	t.clear();

	readOBJ("assets/caret3.obj", v, n, t);
	std::cout << "Caret: " << v.size() << " " << n.size() << " " << t.size() << std::endl;
	caretverts = v.size();
	glGenVertexArrays(1, &caretVao);
	glBindVertexArray(caretVao);
	GLuint caretvbo[2];
	glGenBuffers(2, caretvbo);

	glBindBuffer(GL_ARRAY_BUFFER, caretvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, caretverts*sizeof(vec3), &v[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, caretvbo[1]);
	glBufferData(GL_ARRAY_BUFFER, caretverts*sizeof(vec3), &n[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);


}

float rot = 0;

void renderSkybox(mat4 p, mat4 v){
	glUseProgram(shadowmapshader);
	GLfloat points[] = {
	  -1.0f,  1.0f, -1.0f,
	  -1.0f, -1.0f, -1.0f,
	   1.0f, -1.0f, -1.0f,
	   1.0f, -1.0f, -1.0f,
	   1.0f,  1.0f, -1.0f,
	  -1.0f,  1.0f, -1.0f,

	  -1.0f, -1.0f,  1.0f,
	  -1.0f, -1.0f, -1.0f,
	  -1.0f,  1.0f, -1.0f,
	  -1.0f,  1.0f, -1.0f,
	  -1.0f,  1.0f,  1.0f,
	  -1.0f, -1.0f,  1.0f,

	   1.0f, -1.0f, -1.0f,
	   1.0f, -1.0f,  1.0f,
	   1.0f,  1.0f,  1.0f,
	   1.0f,  1.0f,  1.0f,
	   1.0f,  1.0f, -1.0f,
	   1.0f, -1.0f, -1.0f,

	  -1.0f, -1.0f,  1.0f,
	  -1.0f,  1.0f,  1.0f,
	   1.0f,  1.0f,  1.0f,
	   1.0f,  1.0f,  1.0f,
	   1.0f, -1.0f,  1.0f,
	  -1.0f, -1.0f,  1.0f,

	  -1.0f,  1.0f, -1.0f,
	   1.0f,  1.0f, -1.0f,
	   1.0f,  1.0f,  1.0f,
	   1.0f,  1.0f,  1.0f,
	  -1.0f,  1.0f,  1.0f,
	  -1.0f,  1.0f, -1.0f,

	  -1.0f, -1.0f, -1.0f,
	  -1.0f, -1.0f,  1.0f,
	   1.0f, -1.0f, -1.0f,
	   1.0f, -1.0f, -1.0f,
	  -1.0f, -1.0f,  1.0f,
	   1.0f, -1.0f,  1.0f
	};
	GLuint vbocube;
	glGenBuffers (1, &vbocube);
	glBindBuffer (GL_ARRAY_BUFFER, vbocube);
	glBufferData (GL_ARRAY_BUFFER, 3 * 36 * sizeof (float), &points, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbocube);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glUniformMatrix4fv(glGetUniformLocation(shadowmapshader, "V"), 1, GL_FALSE, &v[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shadowmapshader, "P"), 1, GL_FALSE, &p[0][0]);
	glUniform1i(glGetUniformLocation(shadowmapshader, "cubemap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

	glDisable(GL_DEPTH_TEST);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbocube);
	glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
	);

	// Draw the triangles !
	glDrawArrays(lines?GL_LINE_LOOP:GL_TRIANGLES, 0, 36); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray(0);
	glEnable(GL_DEPTH_TEST);
}

void display(){
	double framestart = glfwGetTime();
	double frametime = framestart-lastframe;
//	char cbuf[10];
//	sprintf(cbuf, "%.1f", 1./frametime);
//	glfwSetWindowTitle(window,cbuf);

	glfwGetFramebufferSize(window, &width, &height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
	glUseProgram(phongshader);
	mat4 ProjectionMatrix = perspective(45.0f, float(width) / float(height), 0.1f, 100.0f);;
	mat4 ViewMatrix = lookAt(
			vec3(-5,10,5),
//			vec3(5,0,-5),
			vec3((gridsize/2.)-(gridsize/4.), 0, -(gridsize/2.)+(gridsize/4.)),
//			vec3(0,-5,0),
//			vec3(1,-5,0),
			vec3(0,1,0));
//	ModelMatrix = scale(ModelMatrix, vec3(2));
//	ViewMatrix = rotate(ViewMatrix, rot+= (go? frametime/2.:0.), normalize(vec3(0,1,0)));
//	ViewMatrix = rotate(ViewMatrix, -(rot-1.57f), normalize(vec3(0,1,0)));
	ViewMatrix = rotate(ViewMatrix, cameraRotz, vec3(0,1,0));
	ViewMatrix = rotate(ViewMatrix, cameraRoty, normalize(vec3(0,0,1)));
//	ViewMatrix = translate(ViewMatrix, vec3(0,0,5.19615));

	glUniformMatrix4fv(ViewID, 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix4fv(ProjID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniform4fv(lightID,1 , &light[0]);
//	vec3 camera = vec3(ViewMatrix * vec4(1,1,1,0));
//	std::cout << camera.x << " " << camera.y << " " << camera.z << std::endl;
//	glUniform3fv(cameraPosID, 1, &camera[0]);

//	glUniform1i(glGetUniformLocation(phongshader, "cubemap"), 0);
	glDepthMask(false);
	renderSkybox(ProjectionMatrix, ViewMatrix);
	glDepthMask(true);
	glUseProgram(phongshader);
	glUniform1i(glGetUniformLocation(phongshader, "cubemap[0]"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

	mat4 ModelMatrix = mat4(1.0);
	ModelMatrix = translate(ModelMatrix, vec3(0,-5,0));
	ModelMatrix = rotate(ModelMatrix, -1.570f, vec3(1, 0, 0));
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelMatrix[0][0]);
	mat4 normalMatrix = transpose(inverse(ModelMatrix));
	glUniformMatrix4fv(NormID, 1, GL_FALSE, &normalMatrix[0][0]);
	vec4 diffuse = vec4(.8f, .8f, .8f, 1.f);
	glUniform4fv(diffuseColorID, 1, &diffuse[0]);
	vec4 specular = vec4(.6, 1., .6, 55);
	glUniform4fv(specularColorID, 1, &specular[0]);
//	glBindVertexArray(vaoID[0]);
//	glDrawArrays(GL_TRIANGLES, 0, vcount[0]);
	glGetError();
	glBindVertexArray(fluidvao[0]);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR){
		std::cout << "errror " << err << " " <<  err << std::endl;
	}
//	glEnable(GL_PRIMITIVE_RESTART);

	glDrawElements(
			lines?GL_LINE_LOOP:GL_TRIANGLES,
			3*2*(gridsize-1)*(gridsize-1),
			GL_UNSIGNED_INT,
			0);
	err = glGetError();
	if (err != GL_NO_ERROR){
		std::cout << "error 2 " << err << " " <<  err << std::endl;
	}
//glDisable(GL_PRIMITIVE_RESTART);
	glBindVertexArray(0);


	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	ModelMatrix = mat4(1.0);
	ModelMatrix = translate(ModelMatrix, vec3(10, -5, -10));
	ModelMatrix = rotate(ModelMatrix, rot+= (go? frametime/1.:0.), vec3(0,1,0));
	ModelMatrix = translate(ModelMatrix, vec3(0, -3, 4));
//	ModelMatrix = rotate(ModelMatrix, rot*4, vec3(.707, 0 ,.707));
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelMatrix[0][0]);
	normalMatrix = transpose(inverse(ViewMatrix * ModelMatrix));
	glUniformMatrix4fv(NormID, 1, GL_FALSE, &normalMatrix[0][0]);
	diffuse = vec4(1., .2, .2, 0.f);
	glUniform4fv(diffuseColorID, 1, &diffuse[0]);
	specular = vec4(.08, .08, 0.08, 128);
	glUniform4fv(specularColorID, 1, &specular[0]);
	glBindVertexArray(cubevao[0]);
	glDrawArrays(GL_TRIANGLES, 0, vcount[1]);
	glBindVertexArray(0);
//	for (Triangle t: tri){
//		t.draw();
//	}


//	glDrawArrays

	// drawing the position caret
	// set the position of the caret
	ModelMatrix = translate(mat4(1.0), vec3(caretx, -3, -carety));
	normalMatrix = transpose(inverse(ViewMatrix * ModelMatrix));
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(NormID, 1, GL_FALSE, &normalMatrix[0][0]);
	// set the colour of the caret
	diffuse = vec4(1.0, 0.3, 0.3, 0.f);
	specular = vec4(.8, .7, .7, 64);
	glUniform4fv(diffuseColorID, 1, &diffuse[0]);
	glUniform4fv(specularColorID, 1, &specular[0]);
	// draw the position of the caret
	glBindVertexArray(caretVao);
	glDrawArrays(GL_TRIANGLES, 0, caretverts);
	glBindVertexArray(0);



	lastframe = framestart;
}

void renderCubeMap(vec3 position){
	glGetError();
	mat4 ProjectionMatrix = perspective(90.f*(6.28f/360.f), 1.f, .1f, 100.f);
	glUseProgram(cubemapshader);
//	GLenum err = glGetError();
//	if (err!=GL_NO_ERROR){
//		printf("error code begin: {%d} {%s}\n", err, gluErrorString(err));
//	}
	glUniformMatrix4fv(glGetUniformLocation(cubemapshader, "P"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
//	err = glGetError();
//	if (err!=GL_NO_ERROR){
//		printf("error code uniform P: {%d} {%s}\n", err, gluErrorString(err));
//	}
		glBindFramebuffer(GL_FRAMEBUFFER, cubeTexture);
	for (int i=0; i<6; ++i){

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeTexture, 0);
//		glBindFramebuffer(GL_FRAMEBUFFER, cubeTexture);
//		err = glGetError();
//		if (err!=GL_NO_ERROR){
//			printf("error code start (%d): {%d} {%s}\n", i, err, gluErrorString(err));
//		}
		GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE){
			std::cout << "Cubemap Error: " << std::hex << status << std::endl;
		}
//		err = glGetError();
//		if (err!=GL_NO_ERROR){
//			printf("error code switch (%d): {%d} {%s} {%d} {%s}\n", i, err, gluErrorString(err), status, gluErrorString(status));
//		}
		//glView
		mat4 ViewMatrix = mat4(1.0);
		glClearColor(.2f, .2f, .2f, 1.0);

		switch (i){
		case 0:
			ViewMatrix = lookAt(
					vec3(0,0,0),
					vec3(1,0,0),
					vec3(0,1,0));
			glClearColor(1, 0, 0, 1.0); //red
//			std::cout << "red" << std::endl;
			break;
		case 1:
			ViewMatrix = lookAt(
					vec3(0,0,0),
					vec3(-1,0,0),
					vec3(0,1,0));
			glClearColor(0, 1, 0, 1.0); //green
			break;
		case 2:
			ViewMatrix = lookAt(
					vec3(0,0,0),
					vec3(0,-1,0),
					vec3(0,0,-1));
			glClearColor(0, 0, 1, 1.0); //blue
			break;
		case 3:
			ViewMatrix = lookAt(
					vec3(0,0,0),
					vec3(0,1,0),
					vec3(0,0,1));
			glClearColor(1, 1, 0, 1.0); //yellow
			break;
		case 4:
			ViewMatrix = lookAt(
					vec3(0,0,0),
					vec3(0,0,-1),
					vec3(0,1,0));
			glClearColor(0, 1, 1, 1.0); //light blue
			break;
		case 5:
			ViewMatrix = lookAt(
					vec3(0,0,0),
					vec3(0,0,1),
					vec3(0,1,0));
			glClearColor(1, 0, 1, 1.0); //pink
			break;
		default:
			break;
		};
//		ViewMatrix = translate(ViewMatrix, -position);
		glViewport(0, 0, cubesize, cubesize);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDepthMask(false);
		renderSkybox(ProjectionMatrix, ViewMatrix);
		glDepthMask(true);
		glUseProgram(cubemapshader);
		glUniformMatrix4fv(glGetUniformLocation(cubemapshader, "V"), 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniform4fv(glGetUniformLocation(cubemapshader, "light"),1 , &light[0]);

		mat4 ModelMatrix = mat4(1.0);
		ModelMatrix = translate(ModelMatrix, vec3(10, -5, -10));
		ModelMatrix = rotate(ModelMatrix, rot, vec3(0,1,0));
		ModelMatrix = translate(ModelMatrix, vec3(0, -3, 4));
//		ModelMatrix = rotate(ModelMatrix, rot*4, vec3(.707, 0 ,.707));
		glUniformMatrix4fv(glGetUniformLocation(cubemapshader, "M"), 1, GL_FALSE, &ModelMatrix[0][0]);
		mat4 normalMatrix = transpose(inverse(ViewMatrix * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(cubemapshader, "normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
		vec3 diffuse = vec3(1., .2, .2);
		glUniform3fv(glGetUniformLocation(cubemapshader, "diffuse_colour"), 1, &diffuse[0]);
		vec4 specular = vec4(.08, .08, 0.8, 128);
		glUniform4fv(glGetUniformLocation(cubemapshader, "specular_colour"), 1, &specular[0]);
		glBindVertexArray(cubevao[0]);
//		glDrawArrays(GL_TRIANGLES, 0, vcount[1]);
		glBindVertexArray(0);
//		err = glGetError();
//		if (err!=GL_NO_ERROR){
//			printf("error code end (%d): {%d} {%s}\n", i, err, gluErrorString(err));
//		}


		// drawing the position caret
		// set the position of the caret
		ModelMatrix = translate(mat4(1.0), vec3(caretx, 1, -carety));
		normalMatrix = transpose(inverse(ViewMatrix * ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(cubemapshader, "M"), 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(cubemapshader, "normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
		// set the colour of the caret
		diffuse = vec3(1.0, 0.3, 0.3);
		specular = vec4(.8, .7, .7, 64);
		glUniform3fv(glGetUniformLocation(cubemapshader, "diffuse_colour"), 1, &diffuse[0]);
		glUniform4fv(glGetUniformLocation(cubemapshader, "specular_colour"), 1, &specular[0]);
		// draw the position of the caret
		glBindVertexArray(caretVao);
		glDrawArrays(GL_TRIANGLES, 0, caretverts);
		glBindVertexArray(0);
	}




}

void keyboardListener(GLFWwindow* win, int key, int scancode, int action, int mods) {
	if (action==GLFW_PRESS){
		switch (key){
		case GLFW_KEY_SPACE:
			go = ! go;
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, 1);
			break;
		case GLFW_KEY_Q:
			lines = ! lines;
			break;
		case GLFW_KEY_W:
//			sim->push(gridsize/2, gridsize/2, 4, 5);
			sim->push(caretx, carety, 2, 2);
			std::cout << caretx << " " << carety << std::endl;
			break;
		case GLFW_KEY_S:
//			sim->addSphere(gridsize/2, gridsize/2, 4, 3);
			sim->addSphere(caretx, carety, 2, 2);
			std::cout << caretx << " " << carety << std::endl;
			break;
		case GLFW_KEY_UP:
			caretx ++;
			break;
		case GLFW_KEY_DOWN:
			caretx --;
			break;
		case GLFW_KEY_LEFT:
			carety ++;
			break;
		case GLFW_KEY_RIGHT:
			carety --;
			break;
		case GLFW_KEY_I:
			cameraRoty +=0.1;
			break;
		case GLFW_KEY_J:
			cameraRotz -=0.1;
			break;
		case GLFW_KEY_K:
			cameraRoty -=0.1;
			break;
		case GLFW_KEY_L:
			cameraRotz +=0.1;
			break;
		default:
			break;
		}
	}


}

void renderFB(){

}
