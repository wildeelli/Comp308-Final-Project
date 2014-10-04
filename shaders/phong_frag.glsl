#version 440 core
out vec3 color;
//in vec3 fragmentColor;
in vec3 normal_modelview;
in vec3 vertex_modelview;
in vec3 eye;

in vec4 light_;

void main(){
	// color = vec3(1,0,0);
	vec3 L = vec3(normalize(light_ - vec4(vertex_modelview,1.0)));
	vec3 N = normalize(normal_modelview);
	vec3 E = normalize(eye);
	
	vec3 R = normalize(-reflect(L,N));
	float dist = length(L);
	
	float lambert = max(dot(L, N), 0.0);
	vec3 diffuse = max(vec3(0.8), 1.0)*lambert;
	float spec = pow(max(dot(R,E),0.0),.3*45);
	vec3 specular = vec3(1.0) * spec;
	
	color = diffuse + specular;
}

