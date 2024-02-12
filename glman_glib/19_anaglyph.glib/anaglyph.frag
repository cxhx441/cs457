#version 330 compatibility

uniform sampler2D uLeftUnit, uRightUnit;
uniform float uOffsetS, uOffsetT;
uniform float uRed, uGreen, uBlue;

in vec2 vST;

void
main(  )
{
	vec4 leftColor = texture( uLeftUnit, vST );
	vec4 rightColor = texture( uRightUnit, vST + vec2(uOffsetS, uOffsetT));

	vec3 color = vec3 (left.r, right.gb); // gb is cyan
	color *= vec3(uRed, uGreen, uBlue);
	color = clamp(color, 0., 1.);

	gl_FragColor = vec4( color, 1. );
}
