#version 330 core

#define FRAG_OUTPUT0 0

layout(location = FRAG_OUTPUT0) out vec4 color;

void main(void)
{
	color = vec4(1.0);
}
