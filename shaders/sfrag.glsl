#version 440 core

//layout(location=0) out float fragDepth;
uniform sampler2D visibility;
in vec2 UV;
out vec3 colour;

void main(){
	colour = texture(visibility, UV).rgb;
	//colour = vec3(UV, 0.0);
}