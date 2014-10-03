// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/


#include <GL/glew.h>

//#define GLFW_INCLUDE_GLU
//#include "GLFW/glfw3.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += Line + "\n";
		VertexShaderStream.close();
	}
	
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += Line +"\n";
		FragmentShaderStream.close();
	}
	
	GLint Result = GL_FALSE;
	int InfoLogLength;
	
	//compile past here
	//vertex
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);
	//check
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	if (&VertexShaderErrorMessage[0] == NULL) fprintf(stdout, "No Problem\n");
	else fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	//fragment
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);
	//check
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	if (&FragmentShaderErrorMessage[0] == NULL) fprintf(stdout, "No Problem\n");
	else fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	
//	if (&VertexShaderErrorMessage[0] != NULL || &FragmentShaderErrorMessage[0] != NULL) exit(1);

	//link
	fprintf(stdout, "linking shaders\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);
	
	//check program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	//std::vector<char> ProgramErrorMessage( max(InfoLogLength, 1) );
	char *infolog = new char[InfoLogLength];
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, infolog);
	if (InfoLogLength > 1) fprintf(stdout, "%s\n", infolog);
	delete[] infolog;
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	return ProgramID;
	
}
