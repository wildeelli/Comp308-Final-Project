#version 440 core

//layout(location=1) in vec3 normal_modelspace

in vec3 colour;

out vec3 color;
in vec3 normal_modelview;
//in vec3 normal_modelspace_;
in vec3 vertex_modelview;
in vec3 eye_dir;
in vec3 eye_loc;
in vec4 light_;
flat in mat4 invertedCamera;

vec4 light2 = vec4(0.0, 10.0, -5.0, 1.0);
//(10.5, 10, 2, 1.0)
uniform vec4 specular_colour;
uniform vec4 diffuse_colour;
//uniform vec3 camerapos;
uniform mat4 V;

uniform samplerCube cubemap;

void main(){
	// color = vec3(1,0,0);
	vec3 L = vec3((light_ - vec4(vertex_modelview,1.0)));
	vec3 N = normalize(normal_modelview);
	vec3 E = normalize(eye_dir);
	vec3 I = normalize(eye_loc);
	
	
	float dist = length(L);
	L = normalize(L);
	vec3 R = normalize(-reflect(L,N));
	//float atten = 1.
	
	float lambert = max(dot(L, N), 0.0);
	vec3 diffuse = diffuse_colour.rgb *lambert;
	float spec = pow(max(dot(R,E),0.0),.3*specular_colour.w);
	vec3 specular = specular_colour.rgb * spec;
	
	L = vec3((light2 - vec4(vertex_modelview,1.0)));
	N = normalize(normal_modelview);
	E = normalize(eye_dir);
	
	dist = length(L);
	L = normalize(L);
	R = normalize(-reflect(L,N));
	//float atten = 1.
	
	lambert = max(dot(L, N), 0.0)*0.3;
	diffuse += diffuse_colour.rgb *lambert;
	spec = pow(max(dot(R,E),0.0),.3*specular_colour.w)*0.3;
	specular += specular_colour.rgb * spec;
	
	N = normalize(eye_dir);
	vec3 reflection =  (reflect(I, N));
	reflection = vec3(invertedCamera * vec4(reflection, 0.0));
	reflection = texture(cubemap, reflection).rgb;
	//vec3 refraction = texture(cubemap, refract(E, N, 1.333)).rgb;
	vec3 refraction = refract(I, N, 1.0/1.3333);
	refraction = vec3(invertedCamera * vec4(refraction, 0.0));
	refraction = texture(cubemap, refraction).rgb;
	
	//color = clamp(normalize((vec3(spec)*diffuse)+specular),0.0, 1.0);
	color = clamp(specular, 0.0, 1.0);
	vec3 solid = mix(diffuse+specular, reflection, 0.5);
	//color = clamp(mix(solid, refraction, 0.5), 0.0, 1.0)
	//R = reflect(I, N);;
	if (diffuse_colour.a > .9){
	 //color = mix(texture(cubemap, refraction).rgb, texture(cubemap, reflection).rgb, 0.2);
	 color = clamp(mix(solid, refraction, 0.5), 0.0, 1.0);
	 //color = texture(cubemap, refraction).rgb;
	 //color = (inverse (V) * vec4(reflect(I, N), 1.0)).rgb;
	 //color =  (reflect(E, N));
	 //color = normal_modelview;
	 //color = refraction;
	 }
	//color = colour;
	//color = vec3(1.0);
}

