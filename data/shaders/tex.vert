#version 330

layout(location=0) in vec4 position;
layout(location=1) in vec2 texCoord;

out vec2 vTexCoord;

uniform mat4 model;
uniform mat4 projectionView;

void main()
{
    vTexCoord = texCoord;
    gl_Position = projectionView * model * position;
}
