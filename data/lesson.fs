// в момент загрузки шейдера будут добавлены следующие параметры:
// #version 330 core

// текстура
uniform sampler2D colorTexture;

// блок входящих параметров, которые переданы из вершинного шейдера
in Fragment {
	vec2 texcoord;
} Frag;

// первый цвет пикселя
layout(location = 0) out vec4 color;

void main(void)
{
	// получим цвет пикселя из текстуры по текстурным координатам
	color = texture(colorTexture, Frag.texcoord);
}
