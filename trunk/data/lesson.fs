#version 330 core

uniform sampler2D colorTexture;

in vec2 fragmentTexcoord;

out vec4 color;

void main(void)
{
	color = texture(colorTexture, fragmentTexcoord);
}
