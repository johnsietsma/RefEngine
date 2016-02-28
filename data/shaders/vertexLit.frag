#version 410

in vec3 vPosition;
in vec3 vNormal;

out vec4 outputColor;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 cameraPosition;
uniform float specularPower = 1;

void main()
{
    float d = max(0, dot( normalize(vNormal.xyz), lightDirection ) );
    vec3 e = normalize( cameraPosition - vPosition.xyz );
    vec3 r = reflect( -lightDirection, vNormal.xyz );
    float s = max( 0, dot( e, r ) );
    s = pow( s, specularPower );
    outputColor = vec4(lightColor * d + lightColor * s, 1);
}
