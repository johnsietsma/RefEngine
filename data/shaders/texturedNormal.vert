#version 410

layout(location=0) in vec4 position;
layout(location=2) in vec4 normal;
layout(location=3) in vec4 tangent;
layout(location=7) in vec2 texCoord;

out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBiTangent;

uniform mat4 modelTransform;
uniform mat4 projectionViewTransform;

void main()
{
    vPosition = position.xyz;
    vTexCoord = texCoord;
    vNormal = normal.xyz;
    vTangent = tangent.xyz;
    vBiTangent = cross( vNormal, vTangent ).xyz;

    gl_Position = projectionViewTransform * modelTransform * position;
}
