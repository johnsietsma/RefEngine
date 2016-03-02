#version 410

in vec2 vTexCoord;

out vec4 outputColor;

uniform sampler2D diffuseSampler;

uniform int cellIndex;
uniform int cellCountX;
uniform int cellCountY;

void main()
{
	vec2 uv = vTexCoord;

	// Shrink the 0-1 uv range to a single cell range
	uv.x = uv.x / cellCountX;
	uv.y = uv.y / cellCountY;

	// Figure out how big the cells are in uv space
	float cellUVWidthX = 1.0 / cellCountX;
	float cellUVWidthY = 1.0 / cellCountY;

	// Figure out which cell we're in
	int cellXIndex = cellIndex % cellCountX;
	int cellYIndex = (cellIndex - cellXIndex) / cellCountX;

	// Offset the uvs to put us in the correct cell.
	uv.x += cellUVWidthX * cellXIndex;
	uv.y += cellUVWidthY * cellYIndex;

   outputColor = texture(diffuseSampler, uv);
}
