#version 330 compatibility

uniform float uOffsetS;
uniform sampler2D uTexUnit;
in vec2  vST;

void main()
{
	float s = 1. - vST.s;		// reverse because we are inside the cylinder
	float t = vST.t;

	float sf = fract( s + uOffsetS );
	float tf = t;

	vec3 newcolor = texture( uTexUnit, vec2(sf,tf) ).rgb;
	newcolor.rgb = newcolor.gbr;
	gl_FragColor = vec4( newcolor, 1. );
}
