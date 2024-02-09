#version 330 compatibility

out vec3 vECpos;
out vec4 vColor;
out float vLightIntensity;

const vec3 LIGHTPOS = vec3(2., 0., 0.);


void
main( )
{
	vECpos = ( gl_ModelViewMatrix * gl_Vertex ).xyz;
	vec3 tnorm = normalize( vec3( gl_NormalMatrix * gl_Normal ) );
	vLightIntensity = dot( normalize(LIGHTPOS - vECpos), tnorm );
	vLightIntensity = abs( vLightIntensity );
	vColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
