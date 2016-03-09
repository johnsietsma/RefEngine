#version 330

layout(location=0) in vec4 position;
layout(location=7) in vec2 texCoord;

out vec2 vTexCoord;

uniform mat4 model;
uniform mat4 projectionView;

void main()
{
	/*mat4 reflectMat = mat4(
	    1.0, -2.0,  0.0,  0.0,
	   -2.0, -1.0, -2.0,  0.0,
	    0.0, -2.0, -2.0,  0.0,
	    0.0,  0.0,  0.0,  1.0
	);*/

	mat4 reflectMat = mat4(
	    1.0, -2.0,  0.0,  0.0,
	   -2.0, -1.0, -2.0,  0.0,
	    0.0, -2.0, -2.0,  0.0,
	    0.0,  0.0,  0.0,  1.0
	);

    gl_Position = reflectMat * projectionView * model * position;
}
