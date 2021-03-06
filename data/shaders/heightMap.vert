#version 330

layout(location=0) in vec4 position;
layout(location=7) in vec2 texCoord;

out vec2 vTexCoord;

uniform mat4 modelTransform;
uniform mat4 projectionViewTransform;
uniform sampler2D heightMapSampler;

void main()
{
    vTexCoord = texCoord;

	vec4 pos = position;
	pos.y += texture( heightMapSampler, texCoord ).r * 5;

    gl_Position = projectionViewTransform * modelTransform * pos;
}
