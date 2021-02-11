#version 420

layout(location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;
uniform float resolution;
uniform float radius;
uniform vec2 direction;

//Intensity of the sepia effect
//Lower the number, closer to regular color
uniform float u_Intensity = 0.6;

void main() 
{
float blur = radius/resolution;
	float hstep = direction.x;
	float vstep = direction.y;

	vec4 source = texture(s_screenTex, inUV);


	//	vec4 sum = texture2D( source , vec2( inUV.x - 4.0*blur*hstep , inUV.y - 4.0*blur*vstep )) * 0.0162162162;
	//	sum += texture2D( source , vec2( inUV.x - 3.0*blur*hstep , inUV.y - 3.0*blur*vstep )) * 0.0540540541;
	//	sum += texture2D( source , vec2( inUV.x - 2.0*blur*hstep , inUV.y - 2.0*blur*vstep )) * 0.1216216216;
	//	sum += texture2D( source , vec2( inUV.x - 1.0*blur*hstep , inUV.y - 1.0*blur*vstep )) * 0.1945945946;
	//	sum += texture2D( source , vec2( inUV.x , inUV.y )) * 0.2270270270;
	//	sum += texture2D( source , vec2( inUV.x + 1.0*blur*hstep , inUV.y + 1.0*blur*vstep )) * 0.1945945946;
	//	sum += texture2D( source , vec2( inUV.x + 2.0*blur*hstep , inUV.y + 2.0*blur*vstep )) * 0.1216216216;
	//	sum += texture2D( source , vec2( inUV.x + 3.0*blur*hstep , inUV.y + 3.0*blur*vstep )) * 0.0540540541;
	//	sum += texture2D( source , vec2( inUV.x + 4.0*blur*hstep , inUV.y + 4.0*blur*vstep )) * 0.0162162162;
	//
	//	fragColor = vec4( sum.rgb , 1.0 );

//	vec3 sepiaColor;
//	sepiaColor.r = ((source.r * 0.393) + (source.g * 0.769) + (source.b * 0.189));
//	sepiaColor.g = ((source.r * 0.349) + (source.g * 0.686) + (source.b * 0.168));
//	sepiaColor.b = ((source.r * 0.272) + (source.g * 0.534) + (source.b * 0.131));
//
//	frag_color.rgb = mix(source.rgb, sepiaColor.rgb, u_Intensity);
//	frag_color.a = source.a;
}