#version 410

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 outputColor;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 cameraPosition;
uniform float specularPower = 1;
uniform sampler2D diffuseSampler;

void main()
{
    float d = max(0, dot( normalize(vNormal.xyz), lightDirection ) );
    vec3 e = normalize( cameraPosition - vPosition );
    vec3 r = reflect( -lightDirection, vNormal );
    float s = max( 0, dot( e, r ) );
    s = pow( s, specularPower );
    vec4 lightOutputColor = vec4(lightColor * d + lightColor * s, 1);

   vec4 diffuseColor = texture( diffuseSampler, vTexCoord );

   outputColor = diffuseColor * lightOutputColor;
}
