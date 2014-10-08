#version 440 core
out vec3 color;
in vec3 normal_modelview;
in vec3 vertex_modelview;
in vec3 eye;

in vec4 light_;

uniform vec4 specular_colour;
uniform vec3 diffuse_colour;

void main(){
	// color = vec3(1,0,0);
	vec3 L = vec3(normalize(light_ - vec4(vertex_modelview,1.0)));
	vec3 N = normalize(normal_modelview);
	vec3 E = normalize(eye);
	
	vec3 R = normalize(-reflect(L,N));
	float dist = length(L);
	
	float lambert = max(dot(L, N), 0.0);
	vec3 diffuse = diffuse_colour *lambert;
	float spec = pow(max(dot(R,E),0.0),.3*specular_colour.w);
	vec3 specular = specular_colour.rgb * spec;
	
	color = clamp(specular + diffuse, 0.0, 1.0);
	//color = specular;
	//color = diffuse;
}

