#SHADER VERTEX
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 uMVP;
out vec2 vST;

void main()
{
	gl_Position = uMVP * position;
	vST = texCoord;
}


#SHADER FRAGMENT
#version 330 core
layout(location = 0) out vec4 color;

uniform vec4 uColor;
uniform sampler2D uTexture;

in vec2 vST;

void main()
{
	vec4 tex_color = texture(uTexture, vST);
	color = mix(uColor, tex_color, tex_color.a) ;
}
