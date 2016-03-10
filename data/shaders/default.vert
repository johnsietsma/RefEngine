#version 330

in vec4 position;

uniform mat4 modelTransform;
uniform mat4 projectionViewTransform;

void main()
{
    gl_Position = projectionViewTransform * modelTransform * position;
}
