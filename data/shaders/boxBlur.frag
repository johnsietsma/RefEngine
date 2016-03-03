#version 410

in vec2 vTexCoord;

out vec4 outputColor;

uniform sampler2D diffuseSampler;

vec4 BoxBlur() {
	
	vec2 texel = 1.0f / textureSize(diffuseSampler, 0).xy; 

	// 9-tap box kernel
	vec4 colour = texture(diffuseSampler, vTexCoord); 
	colour += texture(diffuseSampler, vTexCoord + vec2(-texel.x, texel.y)); 
	colour += texture(diffuseSampler, vTexCoord + vec2(-texel.x, 0)); 
	colour += texture(diffuseSampler, vTexCoord + vec2(-texel.x, -texel.y)); 
	colour += texture(diffuseSampler, vTexCoord + vec2(0, texel.y)); 
	colour += texture(diffuseSampler, vTexCoord + vec2(0, -texel.y)); 
	colour += texture(diffuseSampler, vTexCoord + vec2(texel.x, texel.y)); 
	colour += texture(diffuseSampler, vTexCoord + vec2(texel.x, 0)); 
	colour += texture(diffuseSampler, vTexCoord + vec2(texel.x, -texel.y)); 

	return colour / 9; 
}

void main()
{
   outputColor = BoxBlur( );
}
