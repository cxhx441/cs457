#version 330 compatibility

in vec3 vRefractVector;
uniform samplerCube uRefractUnit;

void
main( )
{
	vec4 refractColor = textureCube(uRefractUnit, normalize(vRefractVector));
	gl_FragColor = refractColor;
}
