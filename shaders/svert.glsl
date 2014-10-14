#version 440

layout(location=0) in vec3 vertexPosition_modelspace;

//uniform mat4 depthMVP;
out vec2 UV;

void main(){
	gl_Position = vec4(vertexPosition_modelspace, 1.0);
	UV = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;
}