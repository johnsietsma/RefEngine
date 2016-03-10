#version 410

in vec2 vTexCoord;


out vec4 outputColor;

void main()
{
   outputColor = vec4(vTexCoord,0,1);
}
