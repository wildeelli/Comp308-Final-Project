#version 440 core
layout(location=0) in vec3 vertexPosition_modelspace;
layout(location=1) in vec3 normal_modelspace;
layout(location=3) in vec3 vertex_colour;

uniform vec4 light;

uniform mat4 normalMatrix;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;


out vec3 normal_modelview;
out vec3 vertex_modelview;
out vec3 eye;
out vec4 light_;

void main(){
	vertex_modelview = vec3(V*M*vec4(vertexPosition_modelspace, 1.0));
	normal_modelview = (normalMatrix*vec4(normal_modelspace,0)).xyz; 
	eye = -vertex_modelview; 
	light_ = V*light;
	//colour = vertex_colour;
	gl_Position = P*V*M * vec4(vertexPosition_modelspace, 1.0);
}
