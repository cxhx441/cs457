//SHADER VERTEX
#version 330 compatibility
uniform mat4 uMVP;
uniform mat4 uRadius;
void main()
{
	gl_Position = uMVP * gl_Vertex;
}


//SHADER FRAGMENT
#version 330 compatibility
uniform vec3 uColor;
void main()
{
	gl_FragColor = vec4(uColor, 1);
}
