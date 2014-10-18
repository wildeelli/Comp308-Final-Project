#version 440

layout(location=0) in vec3 vertexPosition_modelspace;


uniform mat4 V;
uniform mat4 P;

out vec3 texcoords;

void main(){
	texcoords  = vertexPosition_modelspace;
	gl_Position = P*V * vec4 (vertexPosition_modelspace, 1.0);
}

//uniform mat4 depthMVP;
//out vec3 UV;

//void main(){
	//gl_Position = vec4(vertexPosition_modelspace, 1.0);
	//UV = (vertexPosition_modelspace.xyz+vec3(1,1,1))/2.0;
//}