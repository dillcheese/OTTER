#version 410

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;

uniform vec3 LightPos;

out vec4 frag_color;

void main() {
	
	//Lecture 5
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor * inColor;

	//diffuse
	vec3 N = normalize (inNormal);
	vec3 lightDir = normalize(LightPos - inPos);
	
	float dif = max(dot (N, lightDir),0.0);
	vec3 diffuse = dif * inColor;

	//attentuation
	float dist = length (LightPos - inPos);
	diffuse = diffuse / dist; // (dist*dist )
	
	//Specular 
	vec3 camPos = vec3(0.0, 0.0, 3.0); //pass this as a uniform from your c++ code 
	float specularStrength = 1.0;
	vec3 camDir = normalize(camPos - inPos);
	vec3 reflectDir = reflect (-lightDir, N);
	
	vec3 halfDir = normalize(lightDir + camDir);
	float blinn = pow(max(dot (N ,halfDir), 0.0), 19); //blinn calculation

	float spec = pow(max(dot (camDir, reflectDir), 0.0), 6); //shininess coeffiecient (lower is shinier)

//	vec3 specular = specularStrength * spec * lightColor;

	vec3 specular = specularStrength * blinn * lightColor; //use blinn phong


	vec3 result = (ambient + diffuse + specular);

	frag_color = vec4(result, 1.0);

}