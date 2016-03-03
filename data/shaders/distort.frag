#version 410

in vec2 vTexCoord;

out vec4 outputColor;

uniform sampler2D diffuseSampler;

vec4 Distort() {
	vec2 mid = vec2(0.5f);
	float distanceFromCentre = distance(vTexCoord, mid);
	vec2 normalizedCoord = normalize(vTexCoord - mid);
	float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.05f;

	vec2 newCoord = mid + bias * normalizedCoord;
	return texture(diffuseSampler, newCoord);
}


void main()
{
   outputColor = Distort( );
}
