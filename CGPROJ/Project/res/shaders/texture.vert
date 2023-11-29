#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 transform;			// model matrix
//uniform mat4 view;			// view matrix
//uniform mat4 projection;	// projection matrix

void main()
{
	gl_Position = transform * vec4(pos.xyz, 1.0);
	TexCoord = texCoord;

	// We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
	//     TexCoord = texCoord;
	//    TexCoord = vec2(texCoord.x , 1.0 - texCoord.y);

}


