#version 420

layout(binding = 0) uniform sampler2D s_screenTex; //Source image


out vec4 frag_color;

in vec2 inUV;

void main() 
{
 
	vec2 TexelSize = 1.0 / textureSize(s_screenTex, 0); // gets size of single texel
	float tex_offset =TexelSize.y;

	//Sample pixels in a horizontal row
	//Weight should add up to 1
	frag_color = vec4(0.0, 0.0, 0.0, 0.0);

	frag_color += texture(s_screenTex, vec2(inUV.x, inUV.y - 4.0 * tex_offset)) * 0.06;
	frag_color += texture(s_screenTex, vec2(inUV.x, inUV.y - 3.0 * tex_offset)) * 0.09;
	frag_color += texture(s_screenTex, vec2(inUV.x, inUV.y - 2.0 * tex_offset)) * 0.12;
	frag_color += texture(s_screenTex, vec2(inUV.x, inUV.y - 		 tex_offset)) * 0.15;
	frag_color += texture(s_screenTex, vec2(inUV.x, inUV.y				   )) * 0.16;
	frag_color += texture(s_screenTex, vec2(inUV.x, inUV.y +		 tex_offset)) * 0.15;
	frag_color += texture(s_screenTex, vec2(inUV.x, inUV.y + 2.0 * tex_offset)) * 0.12;
	frag_color += texture(s_screenTex, vec2(inUV.x, inUV.y + 3.0 * tex_offset)) * 0.09;
	frag_color += texture(s_screenTex, vec2(inUV.x, inUV.y + 4.0 * tex_offset)) * 0.06;
}