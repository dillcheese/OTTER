#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec2 outUV;
layout(location = 4) out vec4 outTexWeight;

uniform mat4 u_ModelViewProjection;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;
uniform vec3 u_LightPos;

uniform vec3 u_HeightCutoff;
uniform vec3 u_InterpolateFactor;

float GetTextureWeight(float height, float a, float b) {
    float t = min(max((height-a) / (b-a), 0), 1);
    return t * t * (3 - 2 * t);
}

void main() {

	gl_Position = u_ModelViewProjection * vec4(inPosition, 1.0);

	// Lecture 5
	// Pass vertex pos in world space to frag shader
	outPos = (u_Model * vec4(inPosition, 1.0)).xyz;

	// Normals
	outNormal = u_NormalMatrix * inNormal;

	// Pass our UV coords to the fragment shader
	outUV = inUV;

	float height	= inUV.x;

	vec3 upper = u_HeightCutoff + u_InterpolateFactor;
	vec3 lower = u_HeightCutoff - u_InterpolateFactor;

	outTexWeight = vec4 (
	GetTextureWeight(height,upper.x,lower.x),
	min(GetTextureWeight(height,lower.x,upper.x), 	GetTextureWeight(height,upper.y,lower.y)),
	min(GetTextureWeight(height,lower.y,upper.y), 	GetTextureWeight(height,upper.z,lower.z)),
	GetTextureWeight(height,lower.z,upper.z));

	///////////
	outColor = inColor;

}

