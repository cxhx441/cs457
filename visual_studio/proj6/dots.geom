#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable 
#extension GL_EXT_geometry_shader4: enable 
layout( points ) in; 
layout( triangle_strip, max_vertices=256 ) out;

out vec3 gNormal;
out float gLightIntensity;

vec3 LIGHTPOS = vec3(10., 10., 10.);
//vec3 LIGHTPOS = (gl_ModelViewMatrix * vec4(10., 10., 10., 1)).xyz;
vec3 ECPosition;

void
make_face(vec4 c1, vec4 c2, vec4 c3, vec4 c4){
	gNormal = cross(c1.xyz - c3.xyz, c2.xyz - c3.xyz);
	if (dot(gNormal, c1.xyz - gl_PositionIn[0].xyz) < 0) gNormal *= -1;
	gNormal = normalize(gl_NormalMatrix * gNormal);

	vec3 c1_light = (gl_ModelViewMatrix * c1).xyz;
	vec3 c2_light = (gl_ModelViewMatrix * c2).xyz;
	vec3 c3_light = (gl_ModelViewMatrix * c4).xyz;
	vec3 c4_light = (gl_ModelViewMatrix * c3).xyz;

	vec4 c1_out = gl_ModelViewProjectionMatrix * c1;
	vec4 c2_out = gl_ModelViewProjectionMatrix * c2;
	vec4 c3_out = gl_ModelViewProjectionMatrix * c4;
	vec4 c4_out = gl_ModelViewProjectionMatrix * c3;


	gl_Position = c1_out;
	gLightIntensity = max(dot(normalize(LIGHTPOS - c1_light), gNormal), 0.0);
	EmitVertex();
	gl_Position = c2_out;
	gLightIntensity = max(dot(normalize(LIGHTPOS - c2_light), gNormal), 0.0);
	EmitVertex();
	gl_Position = c3_out;
	gLightIntensity = max(dot(normalize(LIGHTPOS - c3_light), gNormal), 0.0);
	EmitVertex();
	gl_Position = c4_out;
	gLightIntensity = max(dot(normalize(LIGHTPOS - c4_light), gNormal), 0.0);
	EmitVertex();
	EndPrimitive();
}
void main(){
	float cube_size = 0.25;
	float dt = cube_size / 2.f;
	vec3 v = gl_PositionIn[0].xyz;
	vec4 c1 = vec4(v, 0) + vec4(-dt,  dt,  dt, 1);
	vec4 c2 = vec4(v, 0) + vec4(-dt,  dt, -dt, 1);
	vec4 c3 = vec4(v, 0) + vec4( dt,  dt, -dt, 1);
	vec4 c4 = vec4(v, 0) + vec4( dt,  dt,  dt, 1);

	vec4 c5 = vec4(v, 0) + vec4(-dt, -dt,  dt, 1);
	vec4 c6 = vec4(v, 0) + vec4(-dt, -dt, -dt, 1);
	vec4 c7 = vec4(v, 0) + vec4( dt, -dt, -dt, 1);
	vec4 c8 = vec4(v, 0) + vec4( dt, -dt,  dt, 1);

	make_face(c1, c2, c3, c4);
	make_face(c4, c3, c7, c8);
	make_face(c3, c2, c6, c7);
	make_face(c1, c2, c6, c5);
	make_face(c1, c4, c8, c5);
	make_face(c6, c7, c8, c5);
} 
