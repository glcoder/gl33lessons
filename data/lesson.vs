#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;

out Fragment {
	vec2 texcoord;
} Frag;

void main(void)
{
	gl_Position   = (projectionMatrix  * viewMatrix * modelMatrix) * vec4(position, 1.0);
	Frag.texcoord = texcoord;
}
