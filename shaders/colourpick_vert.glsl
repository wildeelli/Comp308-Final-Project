#version 440

layout(location=0) in vec3 vertexPosition_modelspace;
layout(location=3) in vec3 vertex_colour;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

//out vec3 vertex_modelview;
out vec3 colour;

void main(){
	gl_Position = P*V*M * vec4(vertexPosition_modelspace, 1.0);
	colour = vertex_colour;
}