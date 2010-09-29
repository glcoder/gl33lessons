#version 330 core

uniform mat4 projectionMatrix;

in vec3 position;
in vec3 color;

out vec3 fragmentColor;

void main(void)
{
	gl_Position   = projectionMatrix * vec4(position, 1.0);
	fragmentColor = color;
}
