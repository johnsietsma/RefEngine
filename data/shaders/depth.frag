#version 410

out float fragDepth;

void main()
{
   fragDepth = gl_FragCoord.z;
}
