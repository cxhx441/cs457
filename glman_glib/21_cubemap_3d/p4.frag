#version 330 compatibility

uniform sampler3D u3DNoiseUnit;
uniform float uNoiseFreq, uNoiseAmp;
uniform float uIndexOfRefraction, uReflect_VS_Refract, uColorMix;
uniform samplerCube uReflectUnit, uRefractUnit;

in vec3 vN; // normal vector
in vec3 vE; // point to eye vector
in vec3 vMC;
in vec4 vColor;
//in vec2  vST;

//const float DELTA = 0.01;
const vec4 WHITE = vec4 (1.);

vec3
rotateNormal(float angx, float angy, vec3 n){
	float cx = cos(angx);
	float cy = cos(angy);
	float sx = sin(angx);
	float sy = sin(angy);

	//n.x = 1*n.x + 0*n.y + 0*n.z;
	float temp_y = 0*n.x + cx*n.y - sx*n.z;
	n.z = 0*n.x + sx*n.y + cx*n.z;
	n.y = temp_y;

	float temp_x = cy*n.x + 0*n.y + sy*n.z;
	//n.y = 0*n.x + 1*n.y + 0*n.z;
	n.z = -sy*n.x + 0*n.y + cy*n.z;
	n.x = temp_x;

	return normalize(n);
}

void
main( )
{
	vec4 nvx = texture3D(u3DNoiseUnit, uNoiseFreq * vMC.xyz);
	vec4 nvy = texture3D(u3DNoiseUnit, uNoiseFreq * vec3(vMC.xy, vMC.z + 0.5));
	float angx = nvx.r + nvx.g + nvx.b + nvx.a - 2.; // -1 to 1
	float angy = nvy.r + nvy.g + nvy.b + nvy.a - 2.; // -1 to 1
	angx *= uNoiseAmp;
	angy *= uNoiseAmp;

	vec3 Normal = normalize(vN);
	vec3 Eye = normalize(vE);
	Normal = normalize(gl_NormalMatrix * Normal);
	//Normal = mix(Normal, -Normal, step(0, dot(Eye, Normal))); // make sure normal always points towards the eye
	Normal = rotateNormal(angx, angy, Normal);

	vec3 reflectVector = reflect(Eye, Normal);
	vec4 reflectColor = textureCube(uReflectUnit, reflectVector);
	reflectColor = mix(reflectColor, vColor, uColorMix);

	float eta = 1. / uIndexOfRefraction;
	vec3 refractVector = refract(Eye, Normal, eta);
	vec4 refractColor;
	if ( all( equal( refractVector, vec3(0.) ) ) )
	{
		refractColor = reflectColor; // total internal reflection
	}
	else
	{
	    refractColor = textureCube(uRefractUnit, refractVector);
		refractColor = mix( refractColor, vColor, uColorMix );
	}

	gl_FragColor = mix(reflectColor, refractColor, uReflect_VS_Refract);
	//gl_FragColor = vec4(1, 1, 1, 1);
}
