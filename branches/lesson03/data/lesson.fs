#version 330 core

// текстура
uniform sampler2D colorTexture;

// входящие параметры, которые получены из вершинного шейдера
in vec2 fragTexcoord;

// результирующий цвет пикселя на экране
out vec4 color;

void main(void)
{
	// получим цвет пикселя из текстуры по текстурным координатам
	color = texture(colorTexture, fragTexcoord);
}
