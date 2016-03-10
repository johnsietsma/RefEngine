#version 330

layout(location=0) in vec4 position;
layout(location=7) in vec2 texCoord;

out vec2 vTexCoord;

uniform mat4 modelTransform;
uniform mat4 projectionViewTransform;

void main()
{
    vTexCoord = texCoord;
    gl_Position = projectionViewTransform * modelTransform * position;
}
