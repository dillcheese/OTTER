#version 410

layout(location = 1) in vec3 inColor;

out vec4 frag_color;

void main() { 
	
	frag_color = vec4(inColor, 1.0) * vec4 (0.4,6.0,7.9,0.4)/ vec4 (1.3,0.4,0.2,0.4);
}