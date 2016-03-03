#version 410

layout(location=0) in vec4 position;

uniform mat4 lightProjectionViewNDC;

void main()
{
    gl_Position = lightProjectionViewNDC * position;
}