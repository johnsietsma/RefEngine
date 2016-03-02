#version 330

layout(location=0) in vec4 position;
layout(location=2) in vec4 normal;

out vec3 vPosition;
out vec3 vNormal;

uniform mat4 model;
uniform mat4 projectionView;

void main()
{
    vPosition = position.xyz;
    vNormal = normal.xyz;

    gl_Position = projectionView * model * position;
}
