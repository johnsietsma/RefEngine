#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 texCoord;
layout(location=2) in vec4 normal;
layout(location=3) in vec4 tangent;

out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBiTangent;

uniform mat4 model;
uniform mat4 projectionView;

void main()
{
    vPosition = position.xyz;
    vTexCoord = texCoord;
    vNormal = normal.xyz;
    vTangent = tangent.xyz;
    vBiTangent = cross( vNormal, vTangent ).xyz;

    gl_Position = projectionView * model * position;
}
