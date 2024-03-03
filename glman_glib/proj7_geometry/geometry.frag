#version 330 compatibility

uniform float uKa, uKd, uKs;
uniform int uShine;
uniform bool uUseChroma;
uniform int uRedDepth, uBlueDepth;

in vec3 gE, gL, gN;
in float gMVC_z;

vec3 perFragLighting(vec3, vec3, vec3);
vec3 rainbow( float  );

void main()
{
	vec3 color = vec3(1.);
	if (uUseChroma){
		float t = (2./3.) * ( abs(gMVC_z) - uRedDepth ) / ( uBlueDepth - uRedDepth );
		t = clamp( t, 0., 2./3. );
		color = rainbow( t );
	}
	vec3 ambientRGB = color;
	vec3 diffuseRGB = color;
	vec3 specularRGB = vec3(1, 1, 1);
	color = perFragLighting(ambientRGB, diffuseRGB, specularRGB);
	gl_FragColor = vec4(color, 1);
}

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

vec3 rainbow( float t )
{
	t = clamp( t, 0., 1. );         // 0.00 is red, 0.33 is green, 0.67 is blue

	float r = 1.;
	float g = 0.0;
	float b = 1.  -  6. * ( t - (5./6.) );

	if( t <= (5./6.) )
	{
		r = 6. * ( t - (4./6.) );
		g = 0.;
		b = 1.;
	}

	if( t <= (4./6.) )
	{
		r = 0.;
		g = 1.  -  6. * ( t - (3./6.) );
		b = 1.;
	}

	if( t <= (3./6.) )
	{
		r = 0.;
		g = 1.;
		b = 6. * ( t - (2./6.) );
	}

	if( t <= (2./6.) )
	{
		r = 1.  -  6. * ( t - (1./6.) );
		g = 1.;
		b = 0.;
	}

	if( t <= (1./6.) )
	{
		r = 1.;
		g = 6. * t;
	}

	return vec3( r, g, b );
}
