#version 330

layout(location=0) in vec4 position;
layout(location=2) in vec2 normal;
layout(location=7) in vec2 texCoord;

out vec2 vTexCoord;
out vec2 vNormal;

uniform mat4 modelTransform;
uniform mat4 textureOffsetTransform;
uniform mat4 viewTransform;
uniform mat4 iewTransform;
uniform mat4 projectionTransform;
uniform mat4 projectionViewTransform;

void main()
{
	vTexCoord = texCoord;// (projectionTransform * inverse(viewTransform) * modelTransform * position).xy;
	vNormal = normal;
    gl_Position = projectionViewTransform * modelTransform * position;
}
