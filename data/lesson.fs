#version 330 core

uniform sampler2D colorTexture;

in Fragment {
	vec2 texcoord;
} Frag;

layout(location = 0) out vec4 color;

void main(void)
{
	color = texture(colorTexture, Frag.texcoord);
}
