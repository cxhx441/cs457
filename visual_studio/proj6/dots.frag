#version 330 compatibility

in vec3 gNormal;
in vec3 gLightIntensity;

void
main(){
	
	vec3 color = vec3(1, 0, 0) * gLightIntensity + vec3(0, 0.2, 0);
	gl_FragColor = vec4(color, 1);
}