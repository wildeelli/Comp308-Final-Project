#version 440 core

//layout(location=0) out float fragDepth;
layout(location=0) out vec3 color;
flat in vec3 colour;
//out vec3 color;

void main(){
	color = colour;
}
