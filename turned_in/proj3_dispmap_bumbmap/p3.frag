#version 330 compatibility

uniform float uKa, uKd, uKs; // ambient, diffuse, specular
uniform float uShininess;
uniform vec4 uColor;
uniform sampler3D u3DNoiseUnit;
uniform float uNoiseFreq, uNoiseAmp;

in vec3  vN; // normal vector
in vec3  vL; // point to light vector
in vec3  vE; // point to eye vector
//in vec2  vST;
in vec3 vMC;

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
	vec3 color = uColor.rgb;
	vec4 nvx = texture3D(u3DNoiseUnit, uNoiseFreq * vMC.xyz);
	float angx = nvx.r + nvx.g + nvx.b + nvx.a - 2.; // -1 to 1
	angx *= uNoiseAmp;
	vec4 nvy = texture3D(u3DNoiseUnit, uNoiseAmp * vec3(vMC.xy, vMC.z + 0.5));
	float angy = nvy.r + nvy.g + nvy.b + nvy.a - 2.; // -1 to 1
	angy *= uNoiseAmp;

    // PER-FRAGMENT LIGHTING
	vec3 Normal = rotateNormal(angx, angy, normalize(vN));
	vec3 Light  = normalize(vL);
	vec3 Eye    = normalize(vE);

	vec3 ambient = uKa * color;

	float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = uKd * dd * color;

	float ss = 0.;
	if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		ss = pow( max( dot(Eye,ref), 0. ), uShininess );
	}
	vec3 specular = uKs * ss * WHITE.rgb;

	gl_FragColor = vec4( ambient + diffuse + specular,  1. );
}
