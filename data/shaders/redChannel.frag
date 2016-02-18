#version 410

in vec2 vTexCoord;

out vec4 outputColor;

uniform sampler2D diffuseSampler;

void main()
{
   outputColor = texture(diffuseSampler, vTexCoord).rrrr;
}
