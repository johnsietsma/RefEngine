#version 330

layout(location=0) in vec4 position;
layout(location=1) in vec2 texCoord;

out vec2 vTexCoord;

uniform mat4 modelTransform;
uniform mat4 projectionViewTransform;

void main()
{
    vTexCoord = vec2(texCoord.x, 1-texCoord.y);
    gl_Position = projectionViewTransform * modelTransform * position;
}
