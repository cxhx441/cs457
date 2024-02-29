#version 330 compatibility

uniform float uKa, uKd, uKs;
uniform float uFreq;
uniform sampler2D uCaveWallUnit;

flat in vec3 vLight;
flat in vec3 vEye;
flat in vec3 vNormal;
in vec2 vST;


int shininess = 128;


vec3 cave_color = vec3(0.59, 0.26, 0.04);
vec3 lava_color = vec3(1.0, 0.37, 0.0);

void main()
{
    // PER-FRAGMENT LIGHTING
	vec3 L = normalize(vLight);
	vec3 E = normalize(vEye);
	vec3 N = normalize(-vNormal);

	vec3 ambient = uKa * cave_color;

	float dd = max( dot(N, L), 0. );
	vec3 diffuse = uKd * dd * cave_color;

	float ss = 0.;
	if( dot(N, L) > 0. )
	{
		vec3 ref = normalize(  reflect( -L, N )  );
		ss = pow( max( dot(E,ref),0. ), shininess );
	}
	vec3 specular = uKs * ss * lava_color;

	vec3 color = ambient + diffuse + specular;

	cave_color = texture(uCaveWallUnit, vec2(vST.s* uFreq, vST.t * uFreq)).rgb;

	color *= cave_color;
    gl_FragColor = vec4(color, 1.);
}
