#version 410

layout(location=0) in vec4 position;
layout(location=2) in vec4 normal;

uniform mat4 model;
uniform mat4 projectionView;
uniform mat4 lightProjectionView;

out vec4 vNormal;
out vec4 vShadowCoord;

void main()
{
	vNormal = normal;
	vShadowCoord = lightProjectionView * position;
    gl_Position = projectionView * model * position;
}