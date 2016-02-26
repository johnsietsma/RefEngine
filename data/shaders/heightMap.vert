#version 330

layout(location=0) in vec4 position;
layout(location=1) in vec2 texCoord;

out vec2 vTexCoord;

uniform mat4 model;
uniform mat4 projectionView;
uniform sampler2D heightMapSampler;

void main()
{
    vTexCoord = texCoord;

	vec4 pos = position;
	pos.y += texture( heightMapSampler, texCoord ).r * 5;

    gl_Position = projectionView * model * pos;
}
