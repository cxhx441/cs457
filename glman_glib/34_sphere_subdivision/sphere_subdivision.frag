#version 330 compatibility

uniform float uKa, uKd, uKs;
uniform int uShine;
uniform int uNormalize;

in vec3 gE, gL, gN;

vec3 perFragLighting(vec3, vec3, vec3);

void main()
{
	vec3 ambientRGB = vec3(1, 1, 1);
	vec3 diffuseRGB = vec3(1, 1, 1);
	vec3 specularRGB = vec3(1, 1, 1);
	vec3 color = perFragLighting(ambientRGB, diffuseRGB, specularRGB);
	gl_FragColor = vec4(color, 1);
}




///*
vec3 perFragLighting(vec3 ambientRGB, vec3 diffuseRGB, vec3 specularRGB){
	vec3 L = normalize(gL);
	vec3 E = normalize(gE);
    vec3 N = normalize(gN);

	vec3 ambient = uKa * ambientRGB;

	float dd = max( dot(N, L), 0. );
	vec3 diffuse = uKd * dd * diffuseRGB;

	float ss = 0.;
	if( dot(N, L) > 0. )
	{
		vec3 ref = normalize(  reflect( -L, N )  );
		ss = pow( max( dot(E,ref),0. ), uShine );
	}
	vec3 specular = uKs * ss * specularRGB;

	return ambient + diffuse + specular;
}
//*/
