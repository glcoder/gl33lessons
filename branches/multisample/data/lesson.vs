#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec3 position;
in vec2 texcoord;

out vec2 fragmentTexcoord;

void main(void)
{
	gl_Position      = (projectionMatrix  * viewMatrix * modelMatrix) * vec4(position, 1.0);
	fragmentTexcoord = texcoord;
}
