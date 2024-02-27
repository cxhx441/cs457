#version 330 compatibility

in vec3 vLight;
in vec3 vEye;
in vec3 vNormal;
in vec2 vST;

int shininess = 128;

vec3 color = vec3(1., 1, 0.);
void main()
{
    // PER-FRAGMENT LIGHTING
	vec3 L = normalize(vLight);
	vec3 E = normalize(vEye);
	vec3 N = normalize(vNormal);

	vec3 ambient = 0.2 * color;

	float dd = max( dot(N, L), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = 0.4 * dd * color;

	float ss = 0.;
	if( dot(N, L) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -L, N )  );
		ss = pow( max( dot(E,ref),0. ), shininess );
	}
	vec3 specular = 0.4 * ss * vec3(1);

	vec3 color = vec3(ambient + diffuse + specular);
    gl_FragColor = vec4(color, 1.);
}
