#version 330 compatibility

uniform float uKa, uKd, uKs;
in vec3 vLight;
in vec3 vEye;
in vec3 vNormal;
in vec2 vST;


int shininess = 128;

vec3 lava_color = vec3(1.0, 0.3686, 0.0);
vec3 cave_color = vec3(0.5882, 0.2588, 0.0392);
vec3 dragon_top_color = vec3(.25);
vec3 dragon_bottom_color = vec3(1.);

vec3 up = vec3(0, 1, 0);
vec3 color;

void main()
{
    // PER-FRAGMENT LIGHTING
	vec3 L = normalize(vLight);
	vec3 E = normalize(vEye);
	vec3 N = normalize(vNormal);

	if (dot(N, up) > 0){
		// cave reflection
		color = lava_color -
				(vec3(1.) - cave_color) -
				(vec3(1.) - dragon_top_color);
	}
	else{
		// lava direct
		color = lava_color -
				(vec3(1.) - dragon_bottom_color);
	}
	//color = vec3(1);

	vec3 ambient = uKa * vec3(1.0);

	float dd = max( dot(N, L), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = uKd * dd * vec3(1.);

	float ss = 0.;
	if( dot(N, L) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -L, N )  );
		ss = pow( max( dot(E,ref),0. ), shininess );
	}
	vec3 specular = uKs * ss * vec3(1.);

	color = color * (ambient.r + diffuse.r + specular.r);
    gl_FragColor = vec4(color, 1.);
}
