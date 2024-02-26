#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable 
#extension GL_EXT_geometry_shader4: enable 
layout( points ) in; 
layout( points, max_vertices=256 ) out;

uniform float uColorMix;

void main(){
	//gl_Position = gl_ModelViewProjectionMatrix * vec4(0, 0, 0, 1);
	//gl_PointSize = 100;
	gl_Position = gl_in[0].gl_Position;
//	EmitVertex();
	//gl_Position = gl_ModelViewProjectionMatrix * vec4(1, 0, 0, 1);
	//gl_PointSize = 100;
	//EmitVertex();
	//gl_Position = gl_ModelViewProjectionMatrix * vec4(0, 1, 0, 1);
	//gl_PointSize = 100;
	EmitVertex();
	EndPrimitive();
} 
