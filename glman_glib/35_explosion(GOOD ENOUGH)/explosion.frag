#version 330 compatibility

uniform vec4 uColor;

void main()
{
	gl_FragColor = vec4(vec3(1, 1, 1) * uColor.rgb, 1);
}
