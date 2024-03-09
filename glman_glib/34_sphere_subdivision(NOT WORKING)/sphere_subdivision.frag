#version 330 compatibility

uniform vec4 uColor;
in vec3 gLightIntensity;

void main()
{
	gl_FragColor = vec4(gLightIntensity * uColor.rgb, 1);
}
