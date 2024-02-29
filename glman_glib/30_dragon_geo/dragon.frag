#version 330 compatibility

uniform float uKa, uKd, uKs;
uniform int uShininess;
in vec3 vLight1;
in vec3 vLight2;
in vec3 vEye;
in vec3 vNormal;
in vec2 vST;


vec3 lava_color = vec3(1.0, 0.37, 0.0);
vec3 cave_color = vec3(0.59, 0.26, 0.04);
vec3 dragon_top_color = vec3(.25);
vec3 dragon_bottom_color = vec3(1.);

void main()
{
    // PER-FRAGMENT LIGHTING 1
	vec3 L = normalize(vLight1);
	vec3 E = normalize(vEye);
	vec3 N = normalize(vNormal);

	vec3 ambient = uKa * cave_color;

	float dd = max( dot(N, L), 0. );
	vec3 diffuse = uKd * dd * lava_color;

	float ss = 0.;
	if( dot(N, L) > 0. )
	{
		vec3 ref = normalize(  reflect( -L, N )  );
		ss = pow( max( dot(E,ref),0. ), uShininess );
	}
	vec3 specular = uKs * ss * lava_color;

	vec3 color1 = (ambient + diffuse + specular);


	// PER-FRAGMENT LIGHTING 2
	L = normalize(vLight2);
	ambient = uKa * cave_color;
	dd = max( dot(N, L), 0. );       // only do diffuse if the light can see the point
	diffuse = uKd * dd * lava_color;
	ss = 0.;
	if( dot(N, L) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -L, N )  );
		ss = pow( max( dot(E,ref),0. ), uShininess );
	}
	specular = uKs * ss * lava_color;

	vec3 color2 = (ambient + diffuse + specular);

	vec3 color = color1/2. + color2/2.;
    gl_FragColor = vec4(color, 1.);
}
