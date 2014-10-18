#version 440 core
// layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location=0) in vec3 vertexPosition_modelspace;
layout(location=1) in vec3 normal_modelspace;
//layout(location=2) in vec2 uv_modelspace;
layout(location=3) in vec3 vertex_colour;


uniform vec4 light;

uniform mat4 normalMatrix;
//uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

//uniform vec4 light_worldspace;

//out vec3 fragmentColor;
out vec3 normal_modelview;
//out vec3 normal_modelspace_;
out vec3 vertex_modelview;
out vec3 eye_dir;
out vec3 eye_loc;
out vec4 light_;
out vec3 colour;
flat out mat4 invertedCamera;

void main(){
	// gl_Position.xyz = vertexPosition_modelspace;
	vertex_modelview = vec3(M*vec4(vertexPosition_modelspace, 1.0));
	normal_modelview = vec3(normalMatrix*vec4(normal_modelspace,0.0));
	//normal_modelspace_ = (M * vec4(normal_modelspace, 0.0)).xyz; 
	//light_modelview = light_worldspace;
	eye_loc = vec3( V * M * vec4(vertexPosition_modelspace, 1.0));
	eye_dir = vec3( V * M * vec4(normal_modelspace, 0.0));
	invertedCamera = mat4(inverse(mat3(V)));
	light_ = V*light;
	//normal_modelview = normalize(V*M*vec4(normal_modelspace,0)).xyz;
	colour = vertex_colour;
	gl_Position = P*V*M * vec4(vertexPosition_modelspace, 1.0);
	// texcoord here?
	
}
