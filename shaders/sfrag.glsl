#version 440 core

uniform samplerCube cubemap;
in vec3 texcoords;
out vec4 frag_colour;

void main(){
	frag_colour = texture(cubemap, texcoords);
	//frag_colour = vec4(texcoords, 1.0);
}


////layout(location=0) out float fragDepth;
//uniform samplerCube visibility;
//in vec3 UV;
//out vec3 colour;

//void main(){
//	colour = texture(visibility, UV).rgb;
	//colour = vec3(UV, 0.0);
//}